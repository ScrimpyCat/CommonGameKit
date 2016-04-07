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
