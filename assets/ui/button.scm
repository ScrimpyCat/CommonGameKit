(gui "gui-button"
    (enum!
        "normal"
        "highlighted"
        "pushed"
    )

    (state! "colour" (253 212 73))
    (state! "radius" (quote (/ 18.0 (+ width height))))
    (state! "status" normal)
    (state! "label" (
        (quote (render-rect rect colour radius))  ; normal
        (quote (render-rect rect (lighten colour 5) radius)) ; highlighted
        (quote (render-rect rect (darken colour 5) radius))  ; pushed
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
