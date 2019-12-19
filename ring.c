#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int electing = 1;
	int ID;
	int message[3];
	int x;
	int i;

	if(size < 3)
	{
		printf("Error: Minimum 3 processes are needed to run the simulation\n");
		MPI_Finalize();
		return;
	}
	
	enum states_enum { passive = 0, active = 1 } state;
	enum boolean { false = 0, true = 1 } leader;
	state = passive;
	leader = false;

	srand(time(NULL) + rank);	

	for(i = 0; i < size; i++){
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank == i){
			ID = rand() % 100000000000;
			printf("Process Rank: %d, ID: %d\n", rank, ID);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);

	if (rank == 0){
		x = rank + 1;
		if (x >= size){
			x = 0;
		}
		state = active;
		message[0] = 0;
		message[1] = -1;
		message[2] = ID;
		MPI_Send(message, 3, MPI_INT, x, 0, MPI_COMM_WORLD);
	}

	while (electing){
		MPI_Recv(message, 3, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (message[0] == 1){
			state = passive;
			electing = 0;
		}
		
		else {
			if (message[2] < ID) {
				state = active;
				message[2] = ID;
			}
			else if (message[2] > ID) {
				state = passive;
			}
			else {
				leader = true;
				message[0] = 1;
				message[1] = rank;
			}
		}
		x = rank + 1;
		if (x >= size){
			x = 0;
		}
		MPI_Send(message, 3, MPI_INT, x, 0, MPI_COMM_WORLD);
	}
	
	if (leader){
		printf("\nI am the leader: Process rank %d\n\n", rank);
	}
	
	MPI_Barrier(MPI_COMM_WORLD);

	MPI_Finalize();
	return;
}
