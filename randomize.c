
#define rando(x,y) random((y)-(x)+1)+(x)
#define rand(x,y) ((x==1)?rando(1,(y)/2)+rando(0,(y)/2):rando((x)/2,(y)/2)+rando((x)/2,(y)/2))

void RandomizeEq(void) {
	for (int i=0; i<2; i++) { //for each player
		Data->P[i].Probe[0].UnitWeight 	= rand (150,500);
		Data->P[i].Probe[0].MaxSafety	= rand (80,98);
		Data->P[i].Probe[0].MaxRD	= rand (Data->P[i].Probe[0].MaxSafety - 10,Data->P[i].Probe[0].MaxSafety - 2);
		Data->P[i].Probe[0].InitCost    = rand (3,12);
		Data->P[i].Probe[0].UnitCost	= rand (1,Data->P[i].Probe[0].InitCost/2-1);
		Data->P[i].Probe[0].RDCost	= rand (1,2);

		Data->P[i].Probe[1].UnitWeight 	= rand (Data->P[i].Probe[0].UnitWeight+200,Data->P[i].Probe[0].UnitWeight+600);
		Data->P[i].Probe[1].MaxSafety	= rand (80,98);
		Data->P[i].Probe[1].MaxRD	= rand (Data->P[i].Probe[1].MaxSafety - 10,Data->P[i].Probe[1].MaxSafety - 2);
		Data->P[i].Probe[1].InitCost    = rand (20,30);
		Data->P[i].Probe[1].UnitCost	= rand (2,Data->P[i].Probe[1].InitCost/4);
		Data->P[i].Probe[1].RDCost	= rand (2,4);

		Data->P[i].Probe[2].UnitWeight 	= rand (Data->P[i].Probe[1].UnitWeight+300,Data->P[i].Probe[0].UnitWeight+700);
		Data->P[i].Probe[2].MaxSafety	= rand (80,98);
		Data->P[i].Probe[2].MaxRD	= rand (Data->P[i].Probe[2].MaxSafety - 10,Data->P[i].Probe[2].MaxSafety - 2);
		Data->P[i].Probe[2].InitCost    = rand (25,40);
		Data->P[i].Probe[2].UnitCost	= rand (4,Data->P[i].Probe[2].InitCost/5);
		Data->P[i].Probe[2].RDCost	= rand (2,6);

		Data->P[i].Manned[0].UnitWeight = rand (400,600);
		Data->P[i].Manned[0].MaxSafety	= rand (80,98);
		Data->P[i].Manned[0].MaxRD	= rand (Data->P[i].Manned[0].MaxSafety - 10,Data->P[i].Manned[0].MaxSafety - 2);
		Data->P[i].Manned[0].InitCost   = rand (12,24);
		Data->P[i].Manned[0].UnitCost	= rand (1,Data->P[i].Manned[0].InitCost/4);
		Data->P[i].Manned[0].RDCost	= rand (1,2);

		Data->P[i].Manned[1].UnitWeight = Data->P[i].Manned[0].UnitWeight+rand(500,850);
		Data->P[i].Manned[1].MaxSafety	= rand (80,98);
		Data->P[i].Manned[1].MaxRD	= rand (Data->P[i].Manned[1].MaxSafety - 10,Data->P[i].Manned[1].MaxSafety - 2);
		Data->P[i].Manned[1].InitCost   = rand (18,32);
		Data->P[i].Manned[1].UnitCost	= rand (4,Data->P[i].Manned[1].InitCost/3);
		Data->P[i].Manned[1].RDCost	= rand (1,3);

		Data->P[i].Manned[2].UnitWeight = rand (Data->P[i].Manned[1].UnitWeight+300,Data->P[i].Manned[1].UnitWeight+500);
		Data->P[i].Manned[2].MaxSafety	= rand (80,98);
		Data->P[i].Manned[2].MaxRD	= rand (Data->P[i].Manned[2].MaxSafety - 10,Data->P[i].Manned[2].MaxSafety - 2);
		Data->P[i].Manned[2].InitCost   = rand (24,40);
		Data->P[i].Manned[2].UnitCost	= rand (8,Data->P[i].Manned[2].InitCost/2-2);
		Data->P[i].Manned[2].RDCost	= rand (3,8);

		Data->P[i].Manned[3].UnitWeight = rand(Data->P[i].Manned[1].UnitWeight+300,Data->P[i].Manned[1].UnitWeight+500);
		Data->P[i].Manned[3].MaxSafety	= rand(80,98);
		Data->P[i].Manned[3].MaxRD	=  rand(Data->P[i].Manned[3].MaxSafety - 10,Data->P[i].Manned[3].MaxSafety - 2);
		Data->P[i].Manned[3].InitCost   = rand (45,75);
		Data->P[i].Manned[3].UnitCost	= rand (20,Data->P[i].Manned[3].InitCost/2);
		Data->P[i].Manned[3].RDCost	= rand (5,9);

		Data->P[i].Manned[4].UnitWeight = rand(Data->P[i].Manned[2].UnitWeight*2,Data->P[i].Manned[2].UnitWeight*3.25);
		Data->P[i].Manned[4].MaxSafety	= rand(80,98);
		Data->P[i].Manned[4].MaxRD	=  rand(Data->P[i].Manned[4].MaxSafety - 10,Data->P[i].Manned[4].MaxSafety - 2);
		Data->P[i].Manned[4].InitCost   = rand (45,75);
		Data->P[i].Manned[4].UnitCost	= rand (20,Data->P[i].Manned[4].InitCost/2);
		Data->P[i].Manned[4].RDCost	= rand (6,10);

		Data->P[i].Manned[5].UnitWeight = rand(1200,1700);
		Data->P[i].Manned[5].MaxSafety	= rand(80,98);
		Data->P[i].Manned[5].MaxRD	=  rand(Data->P[i].Manned[5].MaxSafety - 10,Data->P[i].Manned[5].MaxSafety - 2);
		Data->P[i].Manned[5].InitCost   = rand (24,36);
		Data->P[i].Manned[5].UnitCost	= rand (4,Data->P[i].Manned[5].InitCost/2-4);
		Data->P[i].Manned[5].RDCost	= rand (1,4);

		Data->P[i].Manned[6].UnitWeight = rand(800,1200);
		Data->P[i].Manned[6].MaxSafety	= rand(80,98);
		Data->P[i].Manned[6].MaxRD	=  rand(Data->P[i].Manned[6].MaxSafety - 10,Data->P[i].Manned[6].MaxSafety - 2);
		Data->P[i].Manned[6].InitCost   = rand (36,50);
		Data->P[i].Manned[6].UnitCost	= rand (4,Data->P[i].Manned[6].InitCost/3);
		Data->P[i].Manned[6].RDCost	= rand (2,6);

		Data->P[i].Rocket[0].MaxPay	= rand(maxx(Data->P[i].Manned[0].UnitWeight,Data->P[i].Probe[0].UnitWeight),maxx(Data->P[i].Manned[0].UnitWeight,Data->P[i].Probe[0].UnitWeight)+200);
		Data->P[i].Rocket[0].MaxSafety	= rand(80,98);
		Data->P[i].Rocket[0].MaxRD	=  rand(Data->P[i].Rocket[0].MaxSafety - 10,Data->P[i].Rocket[0].MaxSafety - 2);
		Data->P[i].Rocket[0].InitCost   = rand (18,30);
		Data->P[i].Rocket[0].UnitCost	= rand (2,Data->P[i].Rocket[0].InitCost/3);
		Data->P[i].Rocket[0].RDCost	= rand (1,3);

		Data->P[i].Rocket[1].MaxPay	= rand (Data->P[i].Rocket[0].MaxPay+700,Data->P[i].Rocket[0].MaxPay+1100);
		Data->P[i].Rocket[1].MaxSafety	= rand(80,98);
		Data->P[i].Rocket[1].MaxRD	=  rand(Data->P[i].Rocket[1].MaxSafety - 10,Data->P[i].Rocket[1].MaxSafety - 2);
		Data->P[i].Rocket[1].InitCost   = rand (45,75);
		Data->P[i].Rocket[1].UnitCost	= rand (6,Data->P[i].Rocket[1].InitCost/3);
		Data->P[i].Rocket[1].RDCost	= rand (2,6);

		Data->P[i].Rocket[2].MaxPay	= rand (Data->P[i].Rocket[1].MaxPay+1200,Data->P[i].Rocket[1].MaxPay+1800);
		Data->P[i].Rocket[2].MaxSafety	= rand(80,98);
		Data->P[i].Rocket[2].MaxRD	=  rand(Data->P[i].Rocket[2].MaxSafety - 10,Data->P[i].Rocket[2].MaxSafety - 2);
		Data->P[i].Rocket[2].InitCost   = rand (60,100);
		Data->P[i].Rocket[2].UnitCost	= rand (10,Data->P[i].Rocket[2].InitCost/2-20);
		Data->P[i].Rocket[2].RDCost	= rand (4,8);

		Data->P[i].Rocket[3].MaxPay	= rand(Data->P[i].Manned[4].UnitWeight,Data->P[i].Manned[4].UnitWeight*2);
		Data->P[i].Rocket[3].MaxSafety	= rand(80,98);
		Data->P[i].Rocket[3].MaxRD	=  rand(Data->P[i].Rocket[3].MaxSafety - 10,Data->P[i].Rocket[3].MaxSafety - 2);
		Data->P[i].Rocket[3].InitCost   = rand (120,175);
		Data->P[i].Rocket[3].UnitCost	= rand (20,Data->P[i].Rocket[3].InitCost/3-20);
		Data->P[i].Rocket[3].RDCost	= rand (6,10);

		Data->P[i].Rocket[4].MaxPay	= rand(750,1250);
		Data->P[i].Rocket[4].MaxSafety	= rand(80,98);
		Data->P[i].Rocket[4].MaxRD	= rand(Data->P[i].Rocket[4].MaxSafety - 10,Data->P[i].Rocket[4].MaxSafety - 2);
		Data->P[i].Rocket[4].InitCost   = rand (10,15);
		Data->P[i].Rocket[4].UnitCost	= rand (1,Data->P[i].Rocket[4].InitCost-5);
		Data->P[i].Rocket[4].RDCost	= rand (1,4);

		Data->P[i].Misc[0].UnitWeight	= rand(200,400);
		Data->P[i].Misc[0].MaxSafety	= rand(80,98);
		Data->P[i].Misc[0].MaxRD	= rand(Data->P[i].Misc[0].MaxSafety - 10,Data->P[i].Misc[0].MaxSafety - 2);
		Data->P[i].Misc[0].InitCost 	= rand (9,15);
		Data->P[i].Misc[0].UnitCost	= rand (1,Data->P[i].Misc[0].InitCost/2);
		Data->P[i].Misc[0].RDCost	= rand (1,2);

		Data->P[i].Misc[1].UnitWeight	= Data->P[i].Misc[0].UnitWeight*2;
		Data->P[i].Misc[1].MaxSafety	= rand(80,98);
		Data->P[i].Misc[1].MaxRD	= rand(Data->P[i].Misc[1].MaxSafety - 10,Data->P[i].Misc[1].MaxSafety - 2);
		Data->P[i].Misc[1].InitCost 	= rand (15,21);
		Data->P[i].Misc[1].UnitCost	= rand (3,Data->P[i].Misc[1].InitCost/2.5);
		Data->P[i].Misc[1].RDCost	= rand (2,3);

		Data->P[i].Misc[2].UnitWeight	= rand(2000,3000);
		Data->P[i].Misc[2].MaxSafety	= rand(80,98);
		Data->P[i].Misc[2].MaxRD	= rand(Data->P[i].Misc[2].MaxSafety - 10,Data->P[i].Misc[2].MaxSafety - 2);
		Data->P[i].Misc[2].InitCost 	= rand (35,45);
		Data->P[i].Misc[2].UnitCost	= rand (14,Data->P[i].Misc[2].InitCost/2+3);
		Data->P[i].Misc[2].RDCost	= rand (3,5);

		Data->P[i].Misc[3].UnitWeight	= 0;
		Data->P[i].Misc[3].MaxSafety	= rand(80,98);
		Data->P[i].Misc[3].MaxRD	= rand(Data->P[i].Misc[3].MaxSafety - 10,Data->P[i].Misc[3].MaxSafety - 2);
		Data->P[i].Misc[3].InitCost 	= rand (15,21);
		Data->P[i].Misc[3].UnitCost	= 0;
		Data->P[i].Misc[3].RDCost	= 1;

	}



}
