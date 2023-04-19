ESL Homework_2 report / 111501555 李育松

Github link：https://github.com/nickleeair/NTHU_EE6470_ESL/tree/main/HW2
1. Introduction of the Problem and my Solution
  在lab3中，我們首先學習如何使用TLM建立point-to-point的傳遞。也就是引入initiator還有target的觀念，將要傳輸的資料以transaction的形式打包後，透過socket來做data/pointer的傳輸。
接著進一步學會了在initiator(Testbench)還有target(Filter)中間插入一個Bus來當作Router，來處理單一Initiator對多個Target的情況。
而此次作業就是把Homework_1的架構整併成TLM-socket的形式，再利用qk降低模擬時間，最後利用Bus來處理Target端Filter/Memory的控制訊號。
2. Implementation details
  Filter的做法同HW1 一致。
  先如同lab3利用initiator控制資料傳輸，將要傳輸的資料以blocking transport的形式打包後，透過socket來做傳輸。
lab4所做的是把應該傳輸的data/pointer/address先用socket-to-socket的方式傳遞給中間的Bus，接著再透過Bus中的decoder來決定目前傳送的transaction是要丟給Filter或者是Memory做操作。
Input part:
   由於我們的pixels資訊是R,G,B三個通道的量，所以我們會使用3個Buffer來暫存data，並且宣告有效的mask量為3。
     (有效：value=0xff)，之後再將其透過「write_to_socket」的方式，打包成transaction後傳送給Target。（如下圖所示）
 ![image](https://user-images.githubusercontent.com/102891463/231212296-0a63c6b9-d125-441b-9205-e8ecb1295ffb.png)

而在Bus的版本，在Bus端會將傳入的Addres「MEDIAN_MM_BASE + MEDIAN_FILTER_R_ADDR」進行decode，藉此決定Bus(Bus端的initial socket)要將transcation傳到哪一個target的socket中(Filter or Memory)。
而quantum keeper的部分則是先注解掉老師的wait function，僅僅在initiator的do_tran()中加入qk來記錄cycle數，值得一提的是，因爲是在do_tran()加入qk，而do_tran()在read/write socket都會做一次，因此設定的qk值應當考慮原本cycle數的一半，且因爲qk是自己設定的，需要先估算好運算的時間，才能得出比較合理的數值。
 ![image](https://user-images.githubusercontent.com/102891463/231212269-41a6d178-1e9f-4ccc-a4bb-e4d9fb095ef5.png)

而在result部分會重新提及qk對於simulation time的影響，如果qk的值設定過大，simulation time反而不會有下降。
Output part:
  要特別注意的是在lab3、lab4之中，Sobel的輸出是「灰階」，所以僅僅只需要將「相同」的結果送往R,G,B三通道所對應的data pointer就好。但是在此作業中，由於Gaussian Filter的輸出會是「彩色」的所以必須要將Filter三個channel的輸出都利用獨立的data_buffer存起來，再將其分別給到R,G,B三通道對應的data pointer中。（如下圖所示）
 ![image](https://user-images.githubusercontent.com/102891463/231212231-9b3e23a4-8dca-4e4d-9b0e-0b637aa0da13.png)

3. Experimental results
The original picture:
 ![image](https://user-images.githubusercontent.com/102891463/231212035-69e06d60-c71e-4ae7-b31d-93fc2659ba25.png)

The picture after filters：
 ![image](https://user-images.githubusercontent.com/102891463/231212056-f368e2b9-bcb8-4bda-8ee7-e64a88471373.png)

Simulation time for p2p socket：
 ![image](https://user-images.githubusercontent.com/102891463/231212085-b057d37a-ce15-4709-9d08-8e0655999f16.png)

Simulation time for qk：
 ![image](https://user-images.githubusercontent.com/102891463/231212108-8bfb8fc2-a707-41bb-b675-d8ad7b7b230d.png)

Simulation time for TLM bus
 ![image](https://user-images.githubusercontent.com/102891463/231212127-a1240e4a-51d8-4c0a-824f-3d523699d3c5.png)

4. Discussions and conclusions
   這次的作業將SystemC/TLM還有如何有效的做Memory的存取(DMI)給整合起來，且利用quantum keeper很好的降低模擬時間，儘管我還不能透徹的瞭解使用DMI來操作Memory還有使用一般transaction來傳遞資料在效益上到底有多大的差異，但是透過Lab3~Lab4+HW2的練習之後，我對Testbench(Initiator) - Bus、Router(Targer+Initiator) - Filter(Target)的連接有更近一步的認識！
   
5.補充
   因爲忘記做read write count的功能，我在Filters_TLM_BUS_1的版本中加入了read write count的功能。基本上就是在testbench中在讀資料和寫資料後都加了counter來數讀寫次數以下是添加code的截圖
   ![image](https://user-images.githubusercontent.com/102891463/233007393-b441fc5e-2140-45a4-85d7-1ccdc3ea2717.png)
   ![image](https://user-images.githubusercontent.com/102891463/233007443-ae222984-94f4-422b-b7be-ac18ceb30f5e.png)
   
   以及之後重新run的結果
   ![image](https://user-images.githubusercontent.com/102891463/233007574-5f923f56-dc4f-4796-949a-e2495d0d5155.png)





