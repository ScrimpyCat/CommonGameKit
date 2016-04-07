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
    (state! "inside" 0)
    (state! "dragged" 0)

    (render
        (get status label)
    )

    (control
        (on (click left rect) 
            (if @press
                ((status! pushed) (inside! 1) (dragged! 1))
                ((status! highlighted) (if inside on-click) (inside! 0) (dragged! 0))
            )
            ((status! normal) (inside! 0) (dragged! @press))
        )
        (if dragged
            (if inside (on (cursor rect) (status! pushed) (status! highlighted)))
            (on (cursor rect) (status! highlighted) (status! normal))
        )
    )
)
