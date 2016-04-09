(gui "gui-rect"
    (state! "colour" (193 206 209))
    (state! "radius" (quote (/ 18.0 (+ width height))))
    (render (render-rect rect colour radius))
)
