Expression Language
===================

This is the scripting and data format language used by the engine. Originally intended on using Scheme, but due to difficulties with getting the implementation I wanted to use to work and just wanting to quickly move onto the handling the config file, I decided to just roll a basic limited language. As I kept changing bits of it here and there, I ended up keeping it.


Todo
----

* Need to add support for tagged atoms.
* Need to add functionality to create functions within the language itself.
* Convert code over to use new naming rules.
* Write up a JSON file (or some other format) to describe the functions, their inputs, options, atoms, etc.


JSON Descriptive Format
-----------------------

File to be used to generate docs from, use for auto-completion, and possibly even type analysis. It represents the attributes (functions, inputs, arguments, states, enums) for the given scope.

An attribute (aside from arguments) defines the object with these elements: name, description, type, args, functions, options, states, enums, inputs.

An attribute is of form

    {
        "name": string,
        "description": string,
        "examples": array of strings,
        "return": type or array of type,
        "args": array of (array or set/repeat block) of types,
        "functions": array of attributes,
        "options": array of attributes,
        "states": array of attributes,
        "enums": array of attributes,
        "inputs": array of attributes,
        "includes": array of strings
    }

In includes the strings are paths to other JSON files relative to the current one. These are loaded and inserted into the attribute.

In return the array defines the different versions.

In args the first array defines the different versions, the second array or block defines the types specific to that version.

Arg block has the form

    {
        "set": type or array of types,
        "repeat": type or array of types
    }

While a type takes the form

    {
        "type": string, //atom, string, number, integer, float, list, expression, custom
        "value": value, //default value
        "optional": boolean
    }

Type values take the form of

    { "type": "atom", "value": string }
    { "type": "option", "value": string }
    { "type": "string", "value": string }
    { "type": "number", "value": integer or float }
    { "type": "integer", "value": integer }
    { "type": "float", "value": float }
    { "type": "list", "value": array of types }
    { "type": "expression", "value": nil }
    { "type": "custom", "value": nil }

For custom type names, to provide a relevant name the type can be suffixed with that name. e.g. `custom:name`, `{ "type": "custom:texture" }`. For all the standard types that use string values, the values are formatted to follow the naming conventions. e.g. `{ "type": "atom", "value": "test" } //=> :test`, `{ "type": "atom", "value": ":test" } //=> ::test`.

Examples

    {//base level
        "functions": [
            {
                "name": "+",
                "return": { "type": "number" },
                "args": [
                    { "repeat": [{ "type": "number" }] }
                ]
            },
            {
                "name": "gui-button",
                "states": [
                    {
                        "name": "colour",
                        "return": { "type": "list" },
                        "args": [[
                            { "type": "number" },
                            { "type": "number" },
                            { "type": "number" },
                            { "type": "number", "optional": true }
                        ]],
                        "description": "The colour to be used for the button."
                    }
                ]
            },
            {
                "name": "library",
                "args": [[{ "type": "string" }]],
                "options": [
                    {
                        "name": "source",
                        "args": [
                            [{ "type": "string" }, { "type": "atom", "value": "vertex" }, { "type": "list", "value": [{ "type": "option", "value": "dir" }, { "type": "string" }] }],
                            [{ "type": "string" }, { "type": "atom", "value": "fragment" }, { "type": "list", "value": [{ "type": "option", "value": "dir" }, { "type": "string" }] }]
                        ]
                    }
                ]
            },
            {
                "name": "loop",
                "args": [[
                    { "type": "string", "name": "@item" },
                    { "type": "list" },
                    { "type": "expression" }
                ]]
            },
            {
                "name": "texture",
                "description": "Loads a texture.",
                "return": { "type": "custom:texture" },
                "args": [
                    [{ "type": "string", "name": "name" }],
                    [{ "type": "string", "name": "name" }, { "type": "atom", "value": "linear" }, { "type": "list", "value": [{ "type": "option", "value": "dir" }, { "type": "string", "name": "path" }] }],
                    [{ "type": "string", "name": "name" }, { "type": "atom", "value": "nearest" }, { "type": "list", "value": [{ "type": "option", "value": "dir" }, { "type": "string", "name": "path" }] }]
                ]
            }
        ]
    }


Naming Rules
------------

* Function: `example` - Use no prefix/suffix and are any name/symbol. e.g. `(+ 2 3)`, `(add 2 3)`
* Atoms: `:example` - Use the prefix ':'. e.g. `(do :stuff)`, `(do :this)`
* Options: `example:` - Use the suffix ':'. e.g. `(do (option: 1))`, `(test: 2)`
* State: `.example` - Use the prefix '.'. e.g. `(state! ".test" 3)`, `(.test! 3)`
* Input: `@example` - Use the prefix '@'. e.g. `(+ @test 2)`
* Enum: `&example` - Use the prefix '&'. e.g. `(enum! "&test")`, `(+ &test 2)`


Docs
----

It uses a bracket (Scheme/Lisp) style syntax, where everything separated by a space (other than strings) is an expression. And everything following `;` are comments.

###Types###

* `1` is an integer value (int32_t)
* `1.0` is a floating point value (float)
* `#f` and `#t` are boolean literals (expressed as integers)
* `atom` is an atom, this is a unique type that will either evaluate to something else or to itself
* `"string"` is a string
* `()` is a list
* Other internal types can be defined by the system

###Functions###

*+* Adds all the inputs together, returning a float is any of the inputs contains a float otherwise returning an integer if all inputs are integers. e.g. `(+ 1 2 3) ;6`, `(+ 1 1.2) ;2.2`

**-** Subtracts all the inputs from the first input, same float/integer behaviour as additions. e.g. `(- 3 2 2) ;-1`, `(- 1 0.5) ;0.5`

__*__ Multiplies all inputs together, same float/integer behaviour as additions. e.g.`(* 2 2 2) ;8`, `(* 2 0.5) ;1`

**/** Divides all inputs from the first input, same float/integer behaviour as additions. e.g. `(/ 2 2 2) ;0`, `(/ 2 2 2.0) ;0.5`

**=** Equality operator, tests if all the inputs are equal to each other. e.g. `(= 1 2 3) ;0`, `(= 1 1 1) ;1`

**!=** In-equality operator, tests if all the inputs are not equal to the first. e.g. `(!= 1 2 3) ;1`, `(!= 1 1 1) ;0`

**<=** Less than or equal operator, tests if all the inputs are equal or larger than the one before it (ascending or equal order). e.g. `(<= 1 2 3) ;1`, `(<= 1 1 1) ;1`

**>=** Larger than or equal operator, tests if all the inputs are equal or smaller than the one before it (descending or equal order). e.g. `(>= 1 2 3) ;0`, `(>= 1 1 1) ;1`

**<** Less than operator, tests if all the inputs are larger than the one before it (ascending order). e.g. `(< 1 2 3) ;1`, `(< 1 1 1) ;0`

**>** Larger than operator, tests if all the inputs are smaller than the one before it (descending order). e.g. `(> 1 2 3) ;0`, `(> 1 1 1) ;0`

**not** Performs a logical not on the given value. e.g. `(not 1) ;0`

**min** Returns the lowest input. e.g. `(min 3 2 4) ;2`

**max** Returns the largest input. e.g. `(max 3 2 4) ;4`

**random** Returns a random value, either a random integer, a random value up to max, or a random value between min and max. e.g. `(random) ;609783379`, `(random 3) ;2`, `(random 2 4) ;4`, `(random 1.0) ;0.483411`

**get** Get an element from a list. e.g. `(get 0 (1 2 3)) ;1`, `(get 2 (1 2 3)) ;3`

**quote** Causes an expression not to be evaluated. e.g. `(quote (+ 2 2)) ;(+ 2 2)`

**unquote** Evaluates an expression. e.g. `(unquote 14) ;14`, `(unquote (quote (+ 2 2))) ;4`

**begin** Defines a sequence of statements with scope. And returns the result from the last statement

    (begin
        (+ 1 1)
        (begin
            (+ 2 2)
        ) ;4
    ) ;4

**if** A conditional branch statement. If the predicate is true (1 or some non-zero value) it will evaluate and return the result of the truth expression, or if the predicate is false (0) it will evaluate and return the result of the false expression. e.g. `(if 1 "true" "false") ;"true"`

**loop** A loop statement. Iterates through the list, assigning the element to the given state variable, and evaluating the expression. e.g. `(loop "var" (1 2) var) ;(1 2)`, `(loop "val" (1 2) (+ val 1)) ;(2 3)`

**state!** Adds the state to the encompassing expression. State can then be referenced and set elsewhere in that expression or nested expressions. State creates a link between the atom name and the value it holds. When referencing state, state in the inner scopes has greater preference to state in the outer scopes. 

    (state! "x" 5)
    x ;5
    (x! 10) ;10
    x ;10
    
    ;hooks
    (state! "y" (quote (* x 2)))
    (x! 2) ;2
    y ;4

    ;scope
    (begin
        (state! "x" 10)
        x ;10
        y ;4
        (begin
            x ;10
            (state! "x" 20)
            x ;20
        )
        x ;10
    )

**enum!** Much the same as state except it makes it more convenient have enum like behaviour when declaring the states. Follows the same behaviour as C enums (with the exception that they may be set) where uninitialized enums count up in increments from previous (or 0), or are initialized with a specific integer.

    (enum!
        "x" ;0
        "y" ;1
        "z" ;2
        ("a" 20) ;20
        "b" ;21
    )

    y ;1
    b ;21

**print** Prints the expressions. e.g. `(print 1) ;CCIOExpressionPrint: 1`

**search** Searches the specified path, for any contents that match the given expression.

    ; Assume contents of "folder/":
    ; folder/1.txt
    ; folder/2.txt
    ; folder/2.png
    ; folder/subfolder/3.txt
    ; folder/subfolder/3.png

    (search "folder/") ;("folder/1.txt" "folder/2.txt" "folder/2.png" "folder/subfolder/" "folder/subfolder/3.txt" "folder/subfolder/3.png")
    (search "folder/" ".txt") ;("folder/1.txt" "folder/2.txt" "folder/subfolder/3.txt")
    (search "folder/" "2.*") ;("folder/2.txt" "folder/2.png")
    (search "folder/" "subfolder/*.*") ;("folder/subfolder/3.txt" "folder/subfolder/3.png")

**eval** Evaluate the script. e.g. `(eval "~/script.scm") ;1`

**super** Evaluates the expression in the former in-use state.

    (begin
        (state! "x" 10)
        (begin
            (state! "x" 20)
            (begin
                (super x) ;10
                (super (super x)) ;x
            )
        )
    )

**strict-super** Evaluates the expression in next level up state.

    (begin
        (state! "x" 10)
        (begin
            (state! "x" 20)
            (begin
                (strict-super x) ;20
                (strict-super (strict-super x)) ;10
            )
        )
    )

**prefix** Checks if the string has the prefix. e.g. `(prefix "My" "My Test") ;1`, `(prefix "My" "Test") ;0`

**suffix** Checks if the string has the suffix. e.g. `(suffix "Test" "My Test") ;1`, `(suffix "Test" "My") ;0`

**filename** Get the filename from a string path. e.g. `(filename "folder/folder/file.txt") ;"file.txt"`

**replace** Replace occurrences of string in the string with another string. e.g. `(replace " " "-" "1 2 3 4 5") ;"1-2-3-4-5"`

**lighten** Lighten the colour by a given amount. Float's are a literal adjustment, while integers are a percentage adjustment. e.g. `(lighten (253 212 73) 5) ;(1.041765 0.872941 0.300588)`

**darken** Darken the colour by a given amount. Float's are a literal adjustment, while integers are a percentage adjustment. e.g. `(darken (253 212 73) 5) ;(0.942549 0.789804 0.271961)`


###Engine Functions###

**window-percent-width** Get a value relative to the size of the window's width. Float's map 0% - 100% with 0.0 - 1.0, while integers use 0 - 100. e.g. `(window-percent-width 50) ;320`

**window-percent-height** Get a value relative to the size of the window's height. Float's map 0% - 100% with 0.0 - 1.0, while integers use 0 - 100. e.g. `(window-percent-height 50) ;320`

**window-width** Get the size of the window's width. e.g. `(window-width) ;640`

**window-height** Get the size of the window's height. e.g. `(window-height) ;640`


###Project Config###

**game** Creates a CCEngineConfig structure.

    (game "Blob Game" ; title
        (default-resolution 640 480) ; resolution or window size
        (default-fullscreen false) ; fullscreen mode
        
        (dir-fonts "font/") ; font directories
        (dir-levels "logic/levels/") ; level directories
        (dir-rules "logic/rules/") ; rule directories
        (dir-textures "graphics/textures/") ; texture directories
        (dir-shaders "graphics/shaders/") ; shader directories
        (dir-sounds "audio/") ; sound directories
        (dir-layouts "ui/") ; layout directories
        (dir-entities "logic/entities/") ; entity directories
        (dir-logs "logs/") ; log directory
        (dir-tmp "tmp/") ; tmp directory
    )

**shader library** Creates a shader library with name.

    (library "mylib"
        (if @opengl (
            (source "vert" vertex (dir "myshader.vs"))
            (source "frag" fragment (dir "myshader.fs"))
        ))
    )

**asset** Register assets with the engine.

    (asset
        (shader "myshader" (mylib vert) (mylib frag))
    )


###GUI Functions###

**percent-width** Get a value relative to the width of the current view. Float's map 0% - 100% with 0.0 - 1.0, while integers use 0 - 100. e.g. `(percent-width 100) ;500`, `(percent-width 50) ;250`

**percent-height** Get a value relative to the height of the current view. Float's map 0% - 100% with 0.0 - 1.0, while integers use 0 - 100. e.g. `(percent-height 100) ;500`, `(percent-height 50) ;250`

**on** Match an event predicate. If the predicate is true (1 or some non-zero value) it will evaluate and return the result of the truth expression, or if the predicate is false (0) it will evaluate and return the result of the false expression. If the predicate is of a different event then the one being tested, neither expression will be evaluated. e.g. `(on (cursor rect) 10 20) ;20`

* **cursor** - Predicate to match when the cursor is position inside the given rect. The `@pos` (`@pos-x` and `@pos-y`) variable is set to the current cursor's position. e.g. `(cursor (0 0 10 10)) ;1`, `(on (cursor (0 0 10 10)) @pos) ; (2 5)`
* **click** - Predicate to match when a click event occurs inside the given rect. Clicks can be specified using combination of `left`, `right`, `middle`, `shift`, `ctrl`, `alt`, `cmd`. The `@press` variable will be set to indicate whether the button is down (0) or released (1). e.g. `(click alt-left (0 0 10 10)) ;1`, `(on (click alt-left (0 0 10 10)) @press) ;1`
* **scroll** - Predicate to match when a scroll event occurs inside the given rect. The `@scroll-delta` (`@scroll-delta-x` and `@scroll-delta-y`) variable is set to the current scroll delta. e.g. `(scroll (0 0 10 10)) ;1`, `(on (scroll (0 0 10 10)) @scroll-delta) ;0.1`
* **drop** - Predicate to match when a file drop event occurs inside the given rect. The `@drop-files` variable is set to the list of files being dropped. e.g. `(drop (0 0 10 10)) ;1`, `(on (drop (0 0 10 10)) @drop-files) ;("~/blah.txt")`

**gui** Create a reusable GUI element. It will create a GUI constructor function for the name specified, with the default settings.

    (gui "gui-button"
        (enum!
            "normal"
            "highlighted"
            "pushed"
        )

        (state! "status" normal)
        (state! "label" (
            (quote (render-rect rect (253 212 73)))  ; normal
            (quote (render-rect rect (255 221 108))) ; highlighted
            (quote (render-rect rect (238 190 30)))  ; pushed
        ))
        (state! "on-click" (quote (print "Clicked!")))
        (state! "inside" #f)
        (state! "dragged" #f)

        (render
            (get status label)
        )

        (control
            (on (click left rect)
                (if @press
                    ((status! pushed) (inside! #t) (dragged! #t))
                    ((status! highlighted) (if inside on-click) (inside! #f) (dragged! #f))
                )
                ((status! normal) (inside! #f) (dragged! @press))
            )
            (if dragged
                (if inside (on (cursor rect) (status! pushed) (status! highlighted)))
                (on (cursor rect) (status! highlighted) (status! normal))
            )
        )
    )

    ; Usage:
    (gui-button
        (rect! 0 0 (quote (super (percent-width 50))) (quote (super (percent-height 50))))
    )

