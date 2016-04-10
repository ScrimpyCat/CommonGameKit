(gui "gui-toggle"
    (enum!
        "off"
        "on"
        "highlighted"
        "pushed"
    )

    (state! "off-colour" (234 67 106))
    (state! "on-colour" (138 227 65))
    (state! "colour" (quote (get active (off-colour on-colour))))
    (state! "radius" (quote (/ 18.0 (+ width height))))
    (state! "status" off)
    (state! "active" status)
    (state! "label" (
        (quote (render-rect rect colour radius))  ; off
        (quote (render-rect rect colour radius))  ; on
        (quote (render-rect rect (lighten colour 5) radius)) ; highlighted
        (quote (render-rect rect (darken colour 5) radius))  ; pushed
    ))
    (state! "off-click" (quote (print "Clicked Off!")))
    (state! "on-click" (quote (print "Clicked On!")))
    (state! "inside" #f)
    (state! "dragged" #f)

    (render
        (get status label)
    )

    (control
        (on (click left rect)
            (if @press
                ((status! pushed) (inside! #t) (dragged! #t))
                ((status! highlighted) (if inside ((get (active! (not active)) ((quote off-click) (quote on-click))))) (inside! #f) (dragged! #f))
            )
            ((status! active) (inside! #f) (dragged! @press))
        )
        (if dragged
            (if inside (on (cursor rect) (status! pushed) (status! highlighted)))
            (on (cursor rect) (status! highlighted) (status! active))
        )
    )
)
