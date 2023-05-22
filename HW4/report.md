ESL Homework 4 111501555 李育松
Githublink：https://github.com/nickleeair/NTHU_EE6470_ESL/new/main/HW4
1. Introduction of the Problem and my Solution
在這次的作業中主要是將先前已經完成的Filters TLM model稍作修改後，將其嵌入RISC-V的Virtual Platform上面做整個系統的模擬。
2. Implementation details
基本上是將TLM的code貼在basic-sobel的文件夾内，然後在main.cpp開一個新的bus port給我們的module。
    並修改原先的testbench放入sw的main.cpp内，使其能夠使用risc-vp內的DMA-function來傳接資料。
    要注意的是現在整個系統上有13個module，所以在bus的initiator端必須有13個ports才行。
3. Experimental results
If DMA is using	Simulation Time (ns)	#Instruction
True	3196770880	73597612
False	3406486860	85394107
4. Discussions and conclusions
由上述的結果可以明顯的發現，使用DMA來搬運資料可以大幅的減少整個模擬的時間以及CPU core所執行的instruction數目。
    這個結果確實符合預期，因為假設不使用DMA來「直接」獲得資料的話，當Accelator需要使用某筆資料時就需要跟CPU溝通，讓CPU針對內部的register file做出「load/store」的動作。
    而透過DMA讓Accelator直接跟memory溝通的話，則可以省下中間CPU處理繁瑣load/store的多餘程序。故使用DMA可以大幅的提升運算效率。
