For use with this PIR hardware: amazon.com/WWZMDiB-HC-SR501-Exclusive-Raspberry-Electronic/dp/B0CCF3HYT9/ref=sr_1_1_sspa?sr=8-1-spons&sp_csd=d2lkZ2V0TmFtZT1zcF9hdGY&psc=1

Empirically determined:
Tx = ~1.1s
Ti = ~3.6s

60/4.7 = 12.8, so any given minute above 13 events should be trimmed to 13 to remove artifacts.

Tx varies between individual sensors but is always less than Ti, so the "one event per trigger" logic should not be affected by SD write time. 

References: neufeld.newton.ks.us/electronics/?p=208
            learn.adafruit.com/pir-passive-infrared-proximity-motion-sensor?view=all
