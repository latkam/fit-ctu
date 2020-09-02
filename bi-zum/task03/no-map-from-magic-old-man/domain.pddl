;Header and description
(define (domain sailor-domain)

(:types location natural jewellery transport drugs geography people police state bad-habit job)


(:predicates
    (binded-ground ?ground1 ?ground2)
    (binded-water ?water1 ?water2)
    (at ?where)
    (is ?what)
    (has ?what))    

; ==============================GENERAL==============================
(:action build-boat
    :parameters ()
    :precondition (and (has wood))
    :effect (and (not(has wood)) (has boat))
)

(:action build-frigate
    :parameters ()
    :precondition (and (has wood) (has boat) (has golden-grain))
    :effect (and (not(has wood)) (not (has boat)) (not (has golden-grain)) (has frigate))
)

(:action build-caravel
    :parameters ()
    :precondition (and (has wood) (has boat) (has golden-coin))
    :effect (and (not(has wood)) (not (has boat)) (not (has golden-coin)) (has caravel))
)

(:action get-slightly-drunk
    :parameters ()
    :precondition (and (has alcohol) (not (is slightly-drunk)) (not (is drunk)))
    :effect (and (is slightly-drunk) (not(has alcohol)))
)

(:action get-drunk
    :parameters ()
    :precondition (and (has alcohol) (is slightly-drunk))
    :effect (and (not (has alcohol)) (not (is slightly-drunk)) (is drunk))
)

(:action become-alcohol-addict
    :parameters ()
    :precondition (and (has alcohol) (is drunk))
    :effect (and (not (has alcohol)) (is alcohol-addict))
)

; ==============================FOREST==============================
(:action cut-wood-forest
    :parameters ()
    :precondition (and (at forest))
    :effect (and (has wood))
)

(:action cut-flowers
    :parameters ()
    :precondition (and (at forest))
    :effect (and (has flowers))
)

(:action fight-bear
    :parameters ()
    :precondition (and (at forest))
    :effect (and (is resistant) (has bearskin) (is impressive))
)

(:action meet-magic-old-man
    :parameters ()
    :precondition (and (at forest) (has alcohol))
    :effect (and (not(has alcohol)) (has bad-acquaintances)) ;!!! removed (has map)
)

; ==============================RIVER==============================
(:action steal-boat
    :parameters ()
    :precondition (and (at river))
    :effect (and (has boat) (has criminal-record))
)

(:action gold-mining
    :parameters ()
    :precondition (and (at river))
    :effect (and (has golden-grain))
)

(:action cold-bath
    :parameters ()
    :precondition (and (at river))
    :effect (and (not (is slightly-drunk)) (not (is drunk)))
)

; ==============================HARBOUR==============================
(:action work
    :parameters ()
    :precondition (and (at harbour))
    :effect (and (has golden-grain))
)

(:action trade-coconut
    :parameters ()
    :precondition (and (at harbour) (has coconut))
    :effect (and (not (has coconut)) (has golden-coin))
)

(:action trade-bearskin
    :parameters ()
    :precondition (and (at harbour) (has bearskin))
    :effect (and (not (has bearskin)) (has golden-coin))
)

(:action meet-smugglers
    :parameters ()
    :precondition (and (at harbour) (has bad-acquaintances) (has golden-brick))
    :effect (and (is familiar-with-smugglers))
)

; ==============================PUB==============================
(:action buy-alcohol
    :parameters ()
    :precondition (and (at pub) (has golden-grain))
    :effect (and (not (has golden-grain)) (has alcohol))
)

(:action buy-alcohol-for-others
    :parameters ()
    :precondition (and (at pub) (has golden-coin))
    :effect (and (not(has golden-coin)) (has good-acquaintances))
)

(:action fight
    :parameters ()
    :precondition (and (at pub) (is slightly-drunk))
    :effect (and (is resistant))
)

; ==============================CITY==============================
(:action save-up
    :parameters ()
    :precondition (and (at city) (has golden-grain))
    :effect (and (not (has golden-grain)) (has golden-coin) (has good-acquaintances))
)

(:action invest
    :parameters ()
    :precondition (and (at city) (has golden-coin))
    :effect (and (not (has golden-coin)) (has golden-brick) (has good-acquaintances))
)

(:action steal
    :parameters ()
    :precondition (and (at city))
    :effect (and (has golden-coin) (has criminal-record))
)

(:action buy-inguldence
    :parameters ()
    :precondition (and (at city) (has golden-grain))
    :effect (and (not(has criminal-record)) (not (has golden-grain)))
)

(:action community-work
    :parameters ()
    :precondition (and (at city))
    :effect (and (not (has criminal-record)) (is slightly-drunk))
)

; ==============================ACADEMY==============================
(:action study-for-captain
    :parameters ()
    :precondition (and (not(has criminal-record)) (at academy) (has golden-coin))
    :effect (and (is captain) (is impressive))
)

;==============================SEA==============================
(:action defeat-by-pirates
    :parameters ()
    :precondition (and (at sea) (not(is resistant)))
    :effect (and (is resistant)
            (not(has frigate)) (not(has caravel))
            (not(has golden-grain)) (not(has golden-coin)) (not(has golden-brick)))
)

(:action defeat-pirates
    :parameters ()
    :precondition (and (at sea) (is resistant) (has caravel))
    :effect (and (has boat) (has frigate) (has caravel)
            (has golden-grain) (has golden-coin) (has golden-brick)
            (is impressive) (is heroic))
)

(:action join-pirates
    :parameters ()
    :precondition (and (at sea) (has bad-acquaintances))
    :effect (and (is slightly-drunk))
)

(:action dive-for-pearl
    :parameters ()
    :precondition (and (at sea))
    :effect (and (has pearl))
)

(:action cold-bath
    :parameters ()
    :precondition (and (at sea))
    :effect (and (not (is slightly-drunk)) (not(is drunk)))
)

;==============================LIGHTHOUSE==============================
(:action impress-girl
    :parameters ()
    :precondition (and (at lighthouse) (is impressive))
    :effect (and (has impressed-girl))
)

;==============================ISLAND==============================
(:action collect-coconut
    :parameters ()
    :precondition (and (at island))
    :effect (and (has coconut))
)

(:action cut-wood-island
    :parameters ()
    :precondition (and (at island))
    :effect (and (has wood))
)

(:action get-cocaine
    :parameters ()
    :precondition (and (at island) (has map))
    :effect (and (has cocaine))
)

;==============================HAPPY-END==============================
(:action make-ring
    :parameters ()
    :precondition (and (has golden-brick) (has pearl))
    :effect (and (has ring))
)

(:action marriage
    :parameters ()
    :precondition (and (has impressed-girl)
                  (has ring) (has flowers) (has good-acquaintances)
                  (not (is drunk)) (not(is alcohol-addict)) (not(has criminal-record))
                  (at island))
    :effect (and (is happy) (is married) (not(has ring)) (not (has flower)))
)

(:action become-admiral
    :parameters ()
    :precondition (and (is captain) (is heroic)
                  (not(is slightly-drunk)) (not(is drunk))
                  (at academy))
    :effect (and (is happy) (is admiral))
)

(:action become-cocaine-addict
    :parameters ()
    :precondition (and (has cocaine) (is alcohol-addict)
                  (has frigate) (is familiar-with-smugglers) (has golden-brick))
    :effect (and (is happy) (is cocaine-addict))
)

;==============================TRAVEL==============================
(:action go
    :parameters (?from ?to)
    :precondition (and (binded-ground ?from ?to) (at ?from))
    :effect (and (not(at ?from)) (at ?to))
)

(:action sail-boat
    :parameters (?from ?to)
    :precondition (and (has boat) (binded-water ?from ?to) (at ?from))
    :effect (and (not (at ?from)) (at ?to))
)

(:action sail-frigate
    :parameters (?from ?to)
    :precondition (and (has frigate) (binded-water ?from ?to) (at ?from))
    :effect (and (not (at ?from)) (at ?to))
)

(:action sail-caravel
    :parameters (?from ?to)
    :precondition (and (has caravel) (binded-water ?from ?to) (at ?from))
    :effect (and (not (at ?from)) (at ?to))
)
)