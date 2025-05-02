#include "bench_utils/bench_utils.hpp"


#include <boost/json.hpp>
#include <boost/regex.hpp>
#include <string>
#include <string_view>
namespace
{
std::string pretty_print(boost::json::value const &jv, std::string *indent = nullptr)
{
    namespace json = boost::json;
    std::ostringstream os;
    std::string indent_;
    if (!indent)
        indent = &indent_;
    switch (jv.kind())
    {
    case json::kind::object: {
        os << "{\n";
        indent->append(4, ' ');
        auto const &obj = jv.get_object();
        if (!obj.empty())
        {
            auto it = obj.begin();
            for (;;)
            {
                os << *indent << json::serialize(it->key()) << " : ";
                os << pretty_print(it->value(), indent);
                if (++it == obj.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize(indent->size() - 4);
        os << *indent << "}";
        break;
    }

    case json::kind::array: {
        os << "[\n";
        indent->append(4, ' ');
        auto const &arr = jv.get_array();
        if (!arr.empty())
        {
            auto it = arr.begin();
            for (;;)
            {
                os << *indent;
                os << pretty_print(*it, indent);
                if (++it == arr.end())
                    break;
                os << ",\n";
            }
        }
        os << "\n";
        indent->resize(indent->size() - 4);
        os << *indent << "]";
        break;
    }

    case json::kind::string: {
        os << json::serialize(jv.get_string());
        break;
    }

    case json::kind::uint64:
        os << jv.get_uint64();
        break;

    case json::kind::int64:
        os << jv.get_int64();
        break;

    case json::kind::double_:
        os << jv.get_double();
        break;

    case json::kind::bool_:
        if (jv.get_bool())
            os << "true";
        else
            os << "false";
        break;

    case json::kind::null:
        os << "null";
        break;
    }

    if (indent->empty())
        os << "\n";
    return os.str();
}
struct PlotValues
{
    int range;
    double nanoseconds;
    bool operator<(PlotValues const &others) const
    {
        return range < others.range;
    }
};
} // namespace
namespace bench_utils
{

FilteredArgv filter(int argc, char **argv)
{
    auto a = FilteredArgv{{}, argc, ""};

    auto const expresssion = boost::regex(R"""(--vega-plot="?([^"]*)"?)""");
    for (int i = 0; i < argc; ++i)
    {
        auto matches = boost::match_results<std::string::const_iterator>{};
        auto const one_arg = std::string{argv[i]};
        if (boost::regex_match(one_arg, matches, expresssion))
        {
            a.vega_plot_path = matches[1];
        }
        else
        {
            a.argv.push_back(argv[i]);
        }
    }

    a.argc = a.argv.size();
    return a;
}

MyReporter::MyReporter()
{
}

void MyReporter::addReporter(std::unique_ptr<benchmark::BenchmarkReporter> &&rep)
{
    reporter_.push_back(std::move(rep));
}
bool MyReporter::ReportContext(const Context &context)
{
    bool results = true;
    for (auto &r : reporter_)
        results = results && r->ReportContext(context);
    return results;
}
void MyReporter::ReportRuns(const std::vector<Run> &reports)
{
    for (auto &r : reporter_)
        r->ReportRuns(reports);
}
void MyReporter::Finalize()
{
    for (auto &r : reporter_)
        r->Finalize();
}

std::vector<std::unique_ptr<benchmark::BenchmarkReporter>> &MyReporter::GetReporter()
{
    return reporter_;
}

void make_vega_plot(std::string_view benchmark_report_as_json, std::ostream &out)
{
    auto report = boost::json::parse(benchmark_report_as_json);
    std::unordered_map<std::string, std::set<PlotValues>> plot_data;

    for (auto const &r : report.at("benchmarks").as_array())
    {
        std::string name = r.at("name").as_string().c_str();
        std::string base_name = name.substr(0, name.find("/"));
        plot_data[base_name].insert(PlotValues{int(r.at("range").as_double()), r.at("cpu_time").as_double()});
    }
    auto const vega_tpl = R"""( 
{
  "width": 600,
  "height": 400,
  "data": {
    "values": [
    ]
  },
  "mark": {
    "type": "line",
    "point": true
  },
  "encoding": {
    "x": {
      "field": "range", 
      "type": "quantitative", 
      "title": "Range Size",
      "scale": {"type": "log", "base": 2}
    },
    "y": {
      "field": "nanoseconds", 
      "type": "quantitative", 
      "title": "Time (nanoseconds) log scale",
      "scale":{"type":"log","base":2}
    },
    "color": {
      "field": "series", 
      "type": "nominal", 
      "title": "Implementation"
    },
    "tooltip": [
      {"field": "range", "type": "quantitative", "title": "Range Size"},
      {"field": "nanoseconds", "type": "quantitative", "title": "Time (ns)"},
      {"field": "series", "type": "nominal", "title": "Implementation"}
    ]
  },
  "title": "Performance Comparison by Range Size",
  "config": {
    "axis": {
      "labelFontSize": 12,
      "titleFontSize": 14
    }
  }
}
)""";

    std::ostringstream oss;
    oss << "[\n";
    char const *delimiter = " ";
    for (auto const &r : plot_data)
    {
        auto const &base_name = r.first;
        for (auto const &row : r.second)
        {
            oss << std::format(R"""({}{{"range": {}, "nanoseconds": {}, "series": "{}" }})""", delimiter, row.range,
                               row.nanoseconds, base_name);
            delimiter = ",";
        }
    }
    oss << "]\n";
    boost::json::value tpl = boost::json::parse(vega_tpl);
    tpl.at("data").at("values") = boost::json::parse(oss.str());

    out << std::format(R"""(
```vega
{}
```
)""",
                       pretty_print(tpl));
}

std::unique_ptr<MyReporter> make_reporter(std::ostream &out)
{
    auto reporter = std::make_unique<MyReporter>();
    reporter->addReporter(std::unique_ptr<benchmark::BenchmarkReporter>{benchmark::CreateDefaultDisplayReporter()});
    reporter->addReporter(std::make_unique<benchmark::JSONReporter>());
    reporter->GetReporter().back()->SetOutputStream(&out);
    return reporter;
}

} // namespace bench_utils
