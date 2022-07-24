#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>

namespace svg {
    enum class StrokeLineCap
    {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin
    {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    class Rgb {
    public:
        Rgb() = default;

        Rgb(uint8_t red, uint8_t green, uint8_t blue) :
            red(red), green(green), blue(blue)
        {}

        ~Rgb() = default;

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    // Цвет в формате RGBA
    class Rgba {
    public:
        Rgba() = default;

        Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity) :
            red(red), green(green), blue(blue), opacity(opacity)
        {}

        ~Rgba() = default;

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1.0L;
    };


    using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

    inline const Color NoneColor{ "none" };

    struct OstreamSolutionPrinter
    {
        std::ostream& out;

        void operator()(std::monostate) const
        {}
        void operator()(std::string str) const
        {
            out << str;
        }
        void operator()(Rgb rgb) const
        {
            using namespace std::literals;
            out << "rgb("s << std::to_string(rgb.red) << ","s << std::to_string(rgb.green) << ","s << std::to_string(rgb.blue) << ")"s;
        }
        void operator()(Rgba rgba) const
        {
            using namespace std::literals;
            out << "rgba("s << std::to_string(rgba.red) << ","s << std::to_string(rgba.green) << ","s << std::to_string(rgba.blue) << ","s << rgba.opacity << ")"s;
        }
    };

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap);

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join);

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return  static_cast<Owner&>(*this);
        }

        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return static_cast<Owner&>(*this);
        }

        Owner& SetStrokeWidth(double width) {
            width_ = width;
            return static_cast<Owner&> (*this);
        }
        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            line_cap_ = line_cap;
            return static_cast<Owner&> (*this);
        }
        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            line_join_ = line_join;
            return static_cast<Owner&>(*this);
        }

    protected:
        ~PathProps() = default;
        void RenderAttrs(std::ostream& out) const
        {
            using namespace std::literals;

            if (fill_color_)
            {
                out << " fill=\""sv;
                visit(OstreamSolutionPrinter{ out }, *fill_color_);
                out << "\""sv;
            }
            if (stroke_color_)
            {
                out << " stroke=\""sv;
                visit(OstreamSolutionPrinter{ out }, *stroke_color_);
                out << "\""sv;
            }
            if (width_)
            {
                out << " stroke-width=\""sv << *width_ << "\""sv;
            }
            if (line_cap_)
            {
                out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
            }
            if (line_join_)
            {
                out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            }
        }
    private:
        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> width_;
        std::optional<StrokeLineCap> line_cap_;
        std::optional<StrokeLineJoin> line_join_;
    };

    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };


    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);
    private:
        void RenderObject(const RenderContext& contect) const override;
        std::vector<Point> points_;
        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */
    };

    class ObjectContainer {
    public:

        template <typename T>
        void Add(T obj)
        {
            AddPtr(std::make_unique<T>(obj));
        }

    protected:
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    };

    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text> {
    public:
        Text& SetPosition(Point pos);


        Text& SetOffset(Point offset);

        Text& SetFontSize(uint32_t size);

        Text& SetFontFamily(std::string font_family);

        Text& SetFontWeight(std::string font_weight);

        Text& SetData(std::string data);
    private:
        void RenderObject(const RenderContext& context) const override;
        std::string Encode(std::string data) const;
        Point position_;
        Point offset_;
        uint32_t size_ = 1;
        std::string font_family_;
        std::string font_weight_;
        std::string data_;

    };

    class Document : public ObjectContainer {
    public:
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */
        // void Add(???);
        template <typename Obj>
        void Add(Obj obj) {
            objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        }

        void AddPtr(std::unique_ptr<Object>&& obj) {
            objects_.push_back(std::move(obj));
        }

        void Render(std::ostream& out) const;

    private:
        std::vector<std::unique_ptr<Object>>objects_;
    };

}  // namespace svg