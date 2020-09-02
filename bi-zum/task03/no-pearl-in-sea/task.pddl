(define (problem sailor-problem) (:domain sailor-domain)
(:objects
    forest - location river - location harbour - location pub - location city - location
    academy - location sea - location lighthouse - location island - location     
    
    wood - natural flowers - natural bear - natural bearskin - natural coconut - natural 
    
    golden-grain - jewellery golden-coin - jewellery golden-brick - jewellery ring pearl - jewellery
    
    boat - transport frigate - transport caravel - transport
    
    alcohol - drugs cocaine - drugs
    
    map - geography secret-hideout - geography
    
    magic-old-man - people bad-acquaintances - people good-acquaintances - people
    impressed-girl - people
    
    criminal-record - police
    
    happy - state resistant - state impressive - state heroic - state married - state
    
    slightly-drunk - bad-habit drunk - bad-habit alcohol-addict - bad-habit
    familiar-with-smugglers - bad-habit cocaine-addict - bad-habit
    
    captain - job admiral - job
)

(:init
    (binded-ground pub harbour)
    (binded-ground harbour pub)
    (binded-ground academy city)
    (binded-ground city academy)
    (binded-ground forest river)
    (binded-ground river forest)
    (binded-ground harbour river)
    (binded-ground river harbour)
    (binded-ground harbour city)
    (binded-ground city harbour)
    (binded-water harbour lighthouse)
    (binded-water lighthouse harbour)
    (binded-water harbour sea)
    (binded-water sea harbour)
    (binded-water sea lighthouse)
    (binded-water lighthouse sea)
    (binded-water sea island)
    (binded-water island sea)
    (at harbour)
)

;(:goal (and(is married)))
;(:goal (and(is admiral)))
(:goal (and(is cocaine-addict)))
;(:goal (and(is married) (is admiral) (is cocaine-addict)))



)
