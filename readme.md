# nrf5-lf-clock-accuracy-measure

Simple program to test the accuracy of the different LF clock sources in relation to the external HF crystal:
- Internal LF RC oscillator without calibration 
- Internal LF RC oscillator with calibration 
- External LF crystal

After running the sample the output should look something like this:

><info> app: RC accuracy test example started
>
><info> app: LF clock started
>  
><info> app: LF clock started (RC osc)
>  
><info> app: Results: Min/Max: 468/471, Avg: 469.51, Freq: 34078.0, Dev_ppm: 39978.4
>  
><info> app: Results: Min/Max: 468/471, Avg: 469.49, Freq: 34079.7, Dev_ppm: 40029.3
>  
><info> app: Results: Min/Max: 468/471, Avg: 469.48, Freq: 34080.5, Dev_ppm: 40053.6
>  
><info> app: Results: Min/Max: 468/471, Avg: 469.47, Freq: 34081.0, Dev_ppm: 40069.1
>  
><info> app: Results: Min/Max: 468/471, Avg: 469.51, Freq: 34078.2, Dev_ppm: 39984.9
>  
><info> app: RC osc calibrated
>  
><info> app: Results: Min/Max: 474/493, Avg: 488.30, Freq: 32766.5, Dev_ppm: -46.6
>  
><info> app: Results: Min/Max: 475/493, Avg: 488.29, Freq: 32767.7, Dev_ppm: -7.7
>
><info> app: Results: Min/Max: 475/493, Avg: 488.35, Freq: 32763.7, Dev_ppm: -130.5
>
><info> app: Results: Min/Max: 475/493, Avg: 488.30, Freq: 32766.6, Dev_ppm: -42.5
>
><info> app: Results: Min/Max: 475/493, Avg: 488.32, Freq: 32765.4, Dev_ppm: -79.3
>
><info> app: Changing to external RC clock source...
>
><info> app: Done
>
><info> app: Results: Min/Max: 487/489, Avg: 488.28, Freq: 32767.9, Dev_ppm: -3.5
>
><info> app: Results: Min/Max: 487/490, Avg: 488.28, Freq: 32767.8, Dev_ppm: -5.6
>
><info> app: Results: Min/Max: 487/490, Avg: 488.28, Freq: 32767.9, Dev_ppm: -3.5
>
><info> app: Results: Min/Max: 487/490, Avg: 488.28, Freq: 32767.9, Dev_ppm: -3.5
>
><info> app: Results: Min/Max: 487/490, Avg: 488.28, Freq: 32767.9, Dev_ppm: -3.5
>
><info> app: Tests done
