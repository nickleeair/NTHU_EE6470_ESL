ESL Homework3 report 11150155 李育松
Github:https://github.com/VincentChen1017/NTHU_EE6470_ESL/tree/main/Homework_3
1. Introduction of the Problem and my Solution
在這次的作業之中，我試著使用Cadence的Stratus來進行High Level Synthesis的練習。我將前兩次作業實作的Filters修改成能夠合成的版本，並且將其coding style改善，進行Basic、Pipeline、LOOP UNROLL以及將他們綜合使用後進行效能對比。
2. Implementation details
   在進行HLS之前，我們必須注意要將in/out port修改成可以合成以及可以跑Stratus的port。例如System.h檔中的sc_fifo必須改成cynw_p2p以及GaussFilter.h檔中的sc_fifo_in必須改成scynw_p2p<>::in ......。
•	UNROLL: 由於Filter必須進行3x3的input，故會有一個雙迴圈，首先我對他們兩個迴圈進行complete Unroll。
•	PIPELINE: 接著每個迴圈內部其實會進行數值的運算並且累加，故我利用PIPELINE的方式試著提升他們的throughput。
•	UNROLL,PIPELINE: 最後我同時將全部的LOOP做UNROLL並且對它的累加運算做PIPELINE。試著觀察效能變化。
![image](https://github.com/nickleeair/NTHU_EE6470_ESL/assets/102891463/3dff816d-e65f-4c66-90cf-e94fcdb175b4)
Coding style improve_1：在之前的版本，大部分計算會在for loop内進行，但這樣的計算反而會導致合成時，多次進行除法運算，降低latency，因此我將除法運算移出for loop，減少除法運算的次數。
![image](https://github.com/nickleeair/NTHU_EE6470_ESL/assets/102891463/a233e37c-d1d1-433f-abbe-b3362dd02639)
3. Experimental results
	Area(um^2)	Latency(ns)	Throughput(bit/ns)
	
Base implementation	6630.7	49151990	0.032

improved version_1	5319.0	32767990	0.048

UNROLL			6055.4	15073270	0.104

PIPELINE		9860.5	13762550	0.114

UNROLL+PIPELINE		5326.4	15003270	0.105



    由上述的實驗數據可以發現，在經過improve coding style後，合成的面積變小了，且latency和throughput都有提升，我認爲應該是因爲除法器的移出，導致HLS在合成時不需要多次通過除法器導致的。當我們使用BASIC版本的時候，他合成的面積是最小的，但由於datapath沒有進行優化或者是平行調整的關係，Simulation times相較其他三者來著長非常多。
   而將雙迴圈整個UNROLL開後可以發現由於迴圈內部的運算全部平行處理的關係，Simulation times確實有大幅的下降，不過也因為平行運算所需要的硬體數量增加，故面積也上升。
   而PIPELINE的部分則又使Simulation times再度下降。不過當Pipeline切的Stage越多，同時運算的Stage越多，則所需要的硬體數量又是倍數上升，故面積增加了超過2倍的大小。
   我想在設計時，我們應該選擇要特別著重面積或者是timing來在PIPELINE以及UNROLL間做出不同的調整。
4. Discussions and conclusions
   這次的作業讓我學會了使用HLS來進行電路的合成，由於HLS在Datapath上的便利性，可以直接使用各種Directive即可達到各種datapath處理的效果。不過由於內部隱含著很多最佳化的演算法，故最後合成出來的結果可能與我們想像的大相徑庭，或者是根本就很難去想像電路是如何被優化的。故在使用HLS時我認為必須要非常小心，要仔細的評估目前所下的各種Directive才是。

