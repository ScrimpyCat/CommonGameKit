Expression Language
===================

This is the scripting and data format language used by the engine. Originally intended on using Scheme, but due to difficulties with getting the implementation I wanted to use and just wanting to quickly move onto the handling the config file, I decided to just roll a basic limited language. As I kept changing bits of it here and there, I've ended up keeping it for the time being.


Todo
----

Need to add support for tagged types. So allocations can be avoided for integers and atoms. And atom lookups can be much faster.

Need to add functionality to create functions within the language itself.


Docs
----

It uses a bracket (Scheme/Lisp) style syntax, where everything separated by a space (other than strings) is an expression. And everything following `;` are comments.

###Types###

* `1` is an integer value (int32_t)
* `1.0` is a floating point value (float)
* `atom` is an atom, this is a unique type that will either evaluate to something else or to itself
* `"string"` is a string
* `()` is a list
* Other internal types can be defined by the system

###Functions###

*+* Adds all the inputs together, returning a float is any of the inputs contains a float otherwise returning an integer if all inputs are integers. e.g. `(+ 1 2 3) ;6`, `(+ 1 1.2) ;2.2`

**-** Subtracts all the inputs from the first input, same float/integer behaviour as additions. e.g. `(- 3 2 2) ;-1`, `(- 1 0.5) ;0.5`

***** Multiplies all inputs together, same float/integer behaviour as additions. e.g.`(* 2 2 2) ;8`, `(* 2 0.5) ;1`

**/** Divides all inputs from the first input, same float/integer behaviour as additions. e.g. `(/ 2 2 2) ;0`, `(/ 2 2 2.0) ;0.5`

**=** Equality operator, tests if all the inputs are equal to each other. e.g. `(= 1 2 3) ;0`, `(= 1 1 1) ;1`

**!=** In-equality operator, tests if all the inputs are not equal to each other. e.g. `(!= 1 2 3) ;1`, `(!= 1 1 1) ;0`

**<=** Less than or equal operator, tests if all the inputs are equal or larger than the one before it (ascending or equal order). e.g. `(<= 1 2 3) ;1`, `(<= 1 1 1) ;1`

**>=** Larger than or equal operator, tests if all the inputs are equal or smaller than the one before it (descending or equal order). e.g. `(>= 1 2 3) ;0`, `(>= 1 1 1) ;1`

**<** Less than operator, tests if all the inputs are larger than the one before it (ascending order). e.g. `(< 1 2 3) ;1`, `(< 1 1 1) ;0`

**>** Larger than operator, tests if all the inputs are smaller than the one before it (descending order). e.g. `(> 1 2 3) ;0`, `(> 1 1 1) ;0`

**min** Returns the lowest input. e.g. `(min 3 2 4) ;2`

**max** Returns the largest input. e.g. `(max 3 2 4) ;4`

**random** Returns a random value, either a random integer, a random value up to max, or a random value between min and max. e.g. `(random) ;609783379`, `(random 3) ;2`, `(random 2 4) ;4`, `(random 1.0) ;0.483411`

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

**state!** Adds the state to the encompassing expression. State can then be referenced and set elsewhere in that expression or nested expressions. State creates a link between the atom name and the value it holds. When referencing state, state in the inner scopes has greater preference to state in the outer scopes. 

    (state! "x" 5)
    x ;5
    (x 10) ;10
    x ;10
    
    ;hooks
    (state! "y" (quote (* x 2)))
    (x 2) ;2
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
