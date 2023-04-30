# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <ctime>
# include <omp.h>
#include <time.h>
#include <chrono>

using namespace std;

# define NV 22

int main ( int argc, char **argv );
int *Dijk ( int net[NV][NV] );
void Nearest ( int s, int e, int MinD[NV], bool connected[NV], int *d, int *v );
void Initialize ( int net[NV][NV] );
void timestamp ( void );
void UpdateN ( int s, int e, int mv, bool connected[NV], int net[NV][NV], int MinD[NV] );

int main ( int argc, char **argv )
{
  int i;
  int max = 2147483647;
  int j;
  int *MinD;
  int net[NV][NV];

  timestamp ( );


  Initialize ( net );


  auto start = std::chrono::high_resolution_clock::now();

  MinD = Dijk ( net );


  for ( i = 0; i < NV; i++ )
  {
    cout << "  " << setw(2) << i
         << "  " << setw(12) << "Shortest Distance in the network from node 0: " << MinD[i] << "\n";
  }

  delete [] MinD;


   auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by function: "
    << duration.count()<< " microseconds" << std::endl;
  cout << "\n";
  timestamp ( );

  return 0;
}

int *Dijk ( int net[NV][NV] )

{
  int mv;
  int first;
  int thread_id;
  int last;
  int thread_md;
  int my_mv;
  int step;
  int threads;
  bool *connected;
  int i;
  int max = 2147483647;
  int md;
  int *MinD;
  

  connected = new bool[NV];
  connected[0] = true;
  for ( i = 1; i < NV; i++ )
  {
    connected[i] = false;
  }

  MinD = new int[NV];

  for ( i = 0; i < NV; i++ )
  {
    MinD[i] = net[0][i];
  }

  # pragma omp parallel private ( first, thread_id, last, thread_md, my_mv, step ) \
  shared ( connected, md, MinD, mv, threads, net )
  {
    thread_id = omp_get_thread_num ( );
    threads = omp_get_num_threads ( ); 
    first =   (   thread_id       * NV ) / threads;
    last  =   ( ( thread_id + 1 ) * NV ) / threads - 1;
  
    for ( step = 1; step < NV; step++ )
    {

      # pragma omp single 
      {
        md = max;
        mv = -1; 
      }

      Nearest ( first, last, MinD, connected, &thread_md, &my_mv );

      # pragma omp critical
      {
        if ( thread_md < md )  
        {
          md = thread_md;
          mv = my_mv;
        }
      }

      # pragma omp barrier

      # pragma omp single 
      {
        if ( mv != - 1 )
        {
          connected[mv] = true;
          
        }
      }

      # pragma omp barrier

      if ( mv != -1 )
      {
        UpdateN ( first, last, mv, connected, net, MinD );
      }

      #pragma omp barrier
    }

    # pragma omp single
    {
      cout << "\n";
      cout << "  P" << thread_id
           << ": Exiting.\n";
    }
  }

  delete [] connected;

  return MinD;

}



void Initialize ( int net[NV][NV] ) {
  int i;
  int max = 2147483647;
  int j;

  for ( i = 0; i < NV; i++ )  
  {
    for ( j = 0; j < NV; j++ )
    {
      if ( i == j )
      {
        net[i][i] = 0;
      }
      else
      {
        net[i][j] = max;
      }
    }
  }

  net[0][1] = net[1][0] = 75;
  net[0][2] = net[2][0] = 23;
  net[0][3] = net[3][0] = 45;
  net[0][4] = net[4][0] = 88;
  net[0][5] = net[5][0] = 33;
  net[1][2] = net[2][1] = 12;
  net[1][3] = net[3][1] = 67;
  net[1][4] = net[4][1] = 50;
  net[1][5] = net[5][1] = 89;
  net[2][3] = net[3][2] = 42;
  net[2][4] = net[4][2] = 10;
  net[2][5] = net[5][2] = 57;
  net[3][4] = net[4][3] = 31;
  net[3][5] = net[5][3] = 91;
  net[4][5] = net[5][4] = 19;
  net[1][6] = net[6][1] = 57;
  net[2][7] = net[7][2] = 80;
  net[3][8] = net[8][3] = 63;
  net[4][9] = net[9][4] = 25;
  net[5][10] = net[10][5] = 47;
  net[6][7] = net[7][6] = 75;
  net[6][8] = net[8][6] = 90;
  net[6][9] = net[9][6] = 22;
  net[6][10] = net[10][6] = 11;
  net[7][8] = net[8][7] = 38;
  net[7][9] = net[9][7] = 80;
  net[7][10] = net[10][7] = 15;
  net[8][9] = net[9][8] = 95;
  net[8][10] = net[10][8] = 28;
  net[9][10] = net[10][9] = 67;
  net[1][11] = net[11][1] = 33;
  net[2][12] = net[12][2] = 58;
  net[3][13] = net[13][3] = 81;
  net[4][14] = net[14][4] = 20;
  net[5][15] = net[15][5] = 67;
  net[6][16] = net[16][6] = 44;
  net[7][17] = net[17][7] = 55;
  net[8][18] = net[18][8] = 91;
  net[9][19] = net[19][9] = 13;
  net[10][20] = net[20][10] = 75;
  net[11][12] = net[12][11] = 92;
  net[11][13] = net[13][11] = 77;
  net[11][14] = net[14][11] = 47;
  net[11][15] = net[15][11] = 48;

  return;
}

void timestamp ( ) {
# define TIME_SIZE 40

  static char time_buffer[TIME_SIZE];
  const struct std::tm *tm_ptr;
  std::time_t now;

  now = std::time ( NULL );
  tm_ptr = std::localtime ( &now );

  std::strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm_ptr );

  std::cout << time_buffer << "\n";

  return;
# undef TIME_SIZE
}

void Nearest ( int s, int e, int MinD[NV], bool connected[NV], int *d, int *v ){
  int i;
  int max = 2147483647;

  *d = max;
  *v = -1;
  for ( i = s; i <= e; i++ )
  {
    if ( !connected[i] && MinD[i] < *d )
    {
      *d = MinD[i];
      *v = i;
    }
  }
  return;
}

void UpdateN ( int s, int e, int mv, bool connected[NV], int net[NV][NV], int MinD[NV] ){
  int i;
  int max = 2147483647;

  for ( i = s; i <= e; i++ )
  {
    if ( !connected[i] )
    {
      if ( net[mv][i] < max )
      {
        if ( MinD[mv] + net[mv][i] < MinD[i] )  
        {
          MinD[i] = MinD[mv] + net[mv][i];
        }
      }
    }
  }
  return;

}

