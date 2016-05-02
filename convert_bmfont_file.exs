defmodule BMFont do
    def parse(line), do: create_tag(String.strip(line) |> String.split(" ", trim: true))

    defp create_tag(["info"|args]), do: BMFont.Info.parse(args)
    defp create_tag(["common"|args]), do: BMFont.Common.parse(args)
    defp create_tag(["page"|args]), do: BMFont.Page.parse(args)
    defp create_tag(["char"|args]), do: BMFont.Char.parse(args)
    defp create_tag(["chars"|_]), do: nil #isn't needed
    defp create_tag(["kerning"|args]), do: BMFont.Kerning.parse(args)
    defp create_tag(line), do: IO.puts "Unable to parse line: #{line}"

    defmodule Type do
        defmacro __using__(_) do
            quote do
                import Type

                @before_compile unquote(__MODULE__)
                @types []
            end
        end

        defmacro __before_compile__(env) do
            quote do
                defstruct unquote(Enum.map(Module.get_attribute(env.module, :types), fn { _, name, default, _ } -> { name, default } end))

                def parse(args) do
                    Enum.reduce(args, %unquote(env.module){}, unquote({ :fn, [env.module], Enum.map(Module.get_attribute(env.module, :types), fn
                        { cmd, name, _, :string } -> List.first(quote do
                            <<"#{unquote(cmd)}=", arg :: binary>>, struct -> %{ struct | unquote(name) =>  String.strip(arg, ?") }
                        end)
                        { cmd, name, _, :integer } -> List.first(quote do
                            <<"#{unquote(cmd)}=", arg :: binary>>, struct -> %{ struct | unquote(name) =>  String.to_integer(arg) }
                        end)
                        { cmd, name, _, :bool } -> List.first(quote do
                            <<"#{unquote(cmd)}=", arg :: binary>>, struct -> %{ struct | unquote(name) =>  String.to_integer(arg) != 0 }
                        end)
                        { cmd, name, _, :rect } -> List.first(quote do
                            <<"#{unquote(cmd)}=", arg :: binary>>, struct -> %{ struct | unquote(name) =>  Enum.zip([:up, :right, :down, :left], String.split(arg, ",")) |> Map.new }
                        end)
                        { cmd, name, _, :alignment } -> List.first(quote do
                            <<"#{unquote(cmd)}=", arg :: binary>>, struct -> %{ struct | unquote(name) =>  Enum.zip([:horizontal, :vertical], String.split(arg, ",")) |> Map.new }
                        end)
                    end) }))
                end
            end
        end

        defmacro type(name, default, kind), do: quote do: type(unquote(to_string(name)), unquote(name), unquote(default), unquote(kind))

        defmacro type(cmd, name, default, kind), do: quote do: @types [{ unquote(cmd), unquote(name), unquote(Macro.escape(default)), unquote(kind) }|@types]
    end

    defmodule Info do
        use BMFont.Type

        type :face, "", :string
        type :size, 0, :integer
        type :bold, false, :bool
        type :italic, false, :bool
        type :charset, "", :string
        type :unicode, false, :bool
        type "stretchH", :stretch_height, 100, :integer
        type :smooth, false, :bool
        type "aa", :supersampling, 1, :integer
        type :padding, %{ up: 0, right: 0, down: 0, left: 0 }, :rect
        type :spacing, %{ horizontal: 0, vertical: 0 }, :alignment
        type :outline, 0, :integer
    end

    defmodule Common do
        use BMFont.Type

        type "lineHeight", :line_height, 0, :integer
        type :base, 0, :integer
        type "scaleW", :width, 0, :integer
        type "scaleH", :height, 0, :integer
        type :pages, 0, :integer
        type :packed, false, :bool
        type "alphaChnl", :alpha_channel, 0, :integer
        type "redChnl", :red_channel, 0, :integer
        type "greenChnl", :green_channel, 0, :integer
        type "blueChnl", :blue_channel, 0, :integer
    end

    defmodule Page do
        use BMFont.Type

        type :id, 0, :integer
        type :file, "", :string
    end

    defmodule Char do
        use BMFont.Type

        type :id, 0, :integer
        type :x, 0, :integer
        type :y, 0, :integer
        type :width, 0, :integer
        type :height, 0, :integer
        type :xoffset, 0, :integer
        type :yoffset, 0, :integer
        type :xadvance, 0, :integer
        type :page, 0, :integer
        type "chnl", :channel, 0, :integer
    end

    defmodule Kerning do
        use BMFont.Type

        type :first, 0, :integer
        type :second, 0, :integer
        type :amount, 0, :integer
    end
end

defmodule Font do
    def convert_bmfont(args) do
        args = Enum.sort(args, fn
            %BMFont.Info{}, _ -> true
            _, %BMFont.Info{} -> false
            %BMFont.Common{}, _ -> true
            _, %BMFont.Common{} -> false
            %BMFont.Page{}, _ -> true
            _, %BMFont.Page{} -> false
            %BMFont.Char{ id: a }, %BMFont.Char{ id: b } -> a < b
            %BMFont.Char{}, _ -> true
            _, %BMFont.Char{} -> false
            a, b -> a > b
        end)

        %BMFont.Common{ height: tex_height } = Enum.find(args, fn
            %BMFont.Common{} -> true
            _ -> false
        end)

        convert_bmfont(args |> Enum.map(fn
            chr = %BMFont.Char{ y: y, height: h } -> %BMFont.Char{ chr | y: tex_height - y - h }
            arg -> arg
        end), "")
    end

    defp convert_bmfont([], script), do: script <> ")"
    defp convert_bmfont([%BMFont.Info{ face: face, size: size, bold: bold, italic: italic, unicode: unicode }|args], script) do
        style = cond do
            bold and italic -> " (style bold italic)"
            bold -> " (style bold)"
            italic -> " (style italic)"
            true -> ""
        end
        unicode = if unicode, do: "(unicode #t)", else: "(unicode #f)"

        convert_bmfont(args, script <> """
        (font \"#{face}\" #{size}#{style}
            #{unicode}
        """)
    end
    defp convert_bmfont([%BMFont.Common{ line_height: line_height, base: base, pages: 1 }|args], script) do
        convert_bmfont(args, script <> """
            (line-height #{line_height})
            (base #{base})
        """)
    end
    defp convert_bmfont([%BMFont.Page{ file: file }|args], script) do
        convert_bmfont(args, script <> """
            (texture \"#{file}\")
        """)
    end
    defp convert_bmfont([%BMFont.Char{ id: id, x: x, y: y, width: w, height: h, xoffset: xoffset, yoffset: yoffset, xadvance: xadvance }|args], script) do
        letter = case [id] do
            '\\' -> '\\\\'
            '"' -> '\\"'
            c -> c
        end
        convert_bmfont(args, script <> "    (letter \"#{letter}\" (glyph #{x} #{y} #{w} #{h}) (offset #{xoffset} #{yoffset}) (advance #{xadvance}))\n")
    end
    defp convert_bmfont([_|args], script), do: convert_bmfont(args, script)
end

case System.argv do
    [fnt] -> File.stream!(fnt) |> Enum.map(&BMFont.parse/1) |> Font.convert_bmfont |> IO.puts
    _ -> IO.puts "Specify the BMFont file"
end
