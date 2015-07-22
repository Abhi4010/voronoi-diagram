#include<windows.h>
#include<iostream>
#include<stdio.h>
#include <iostream>
#include<stdlib.h>
#include<math.h>
#include <vector>
#include <time.h>
#include<GL/glut.h>

#define pi 2*acos(0)
using namespace std;

double INF = 1e100;
double EPS = 1e-12;
bool delaunayShow = 0;
bool circumcenterShow = 0;
int n;


struct point {
  double x, y;
  point() {}
  point(double x, double y) : x(x), y(y) {}
  point(const point &p) : x(p.x), y(p.y)    {}
  point operator + (const point &p)  const { return point(x+p.x, y+p.y); }
  point operator - (const point &p)  const { return point(x-p.x, y-p.y); }
  point operator * (double c)     const { return point(x*c,   y*c  ); }
  point operator / (double c)     const { return point(x/c,   y/c  ); }
};

double dot(point p, point q)     { return p.x*q.x+p.y*q.y; }
double dist2(point p, point q)   { return dot(p-q,p-q); }
double dist(point p, point q)   { return sqrt(dist2(p,q)); }
double mag(point p) {return dist(p,point(0,0));}
double cross(point p, point q)   { return p.x*q.y-p.y*q.x; }

point RotateCCW90(point p)   { return point(-p.y,p.x); }
point RotateCW90(point p)    { return point(p.y,-p.x); }

point ComputeLineIntersection(point a, point b, point c, point d) {
  b=b-a; d=c-d; c=c-a;
  return a + b*cross(c, d)/cross(b, d);
}
// project point c onto line through a and b
// assuming a != b
point ProjectPointLine(point a, point b, point c) {
  return a + (b-a)*dot(c-a, b-a)/dot(b-a, b-a);
}


point ComputeCircleCenter(point a, point b, point c) {
  b=(a+b)/2;
  c=(a+c)/2;
  return ComputeLineIntersection(b, b+RotateCW90(a-b), c, c+RotateCW90(a-c));
}

vector <point> input,voronoi,circumcenter;
vector <int> delaunay[3];
int tri_count[100][100];

double norm(double x)
{
    if(x<250) return (-(250-x)/250.0);
    return (x-250.0)/250.0;
}

int match(int a,int b,int c,int p,int q,int r)
{
    return ( (a==p || a==q || a==r) +
             (b==p || b==q || b==r) +
            (c==p || c==q || c==r));
}

bool valid(point p)
{
    return (p.x>=0 && p.x<=500 && p.y>=0 && p.y<=500 );
}

void draw_line_unbounded(point a,point b,point c,point p)
{
    point d = a + (b-a)*dot(c-a, b-a)/dot(b-a, b-a);

    double dx,dy;

    dx = d.x-c.x;
    dy = d.y-c.y;

    if(dist(d,p)<=dist(c,p))
    {
        dx*=-1;
        dy*=-1;
    }

    double lo=0,hi=1000,mid;

    while(fabs(hi-lo)<1e-2)
    {
        mid=(lo+hi)/2.0;

        if(valid(point(c.x+mid*dx ,c.y+mid*dy ))) lo=mid;
        else hi=mid;
    }
    mid=(lo+hi)/2.0;

    point r=point(c.x+mid*dx ,c.y+mid*dy );


    glBegin(GL_LINES);
        glVertex2f(norm(c.x), norm(c.y));
        glVertex2f(norm(r.x), norm(r.y));

    glEnd();

}

void display() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClear(GL_COLOR_BUFFER_BIT);

   double del=0.02;

   glColor3f(1.0f, 1.0f, 1.0f);         // Clear the color buffer (background)

//Point draw
   for(int i=0;i<input.size();i++)
	{
	    glPushMatrix();{

	        glTranslatef(norm(input[i].x),norm(input[i].y),0);

            glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
                  // Red
                  glVertex2f(-del, -del);    // x, y
                  glVertex2f( del, -del);
                  glVertex2f( del,  del);
                  glVertex2f(-del,  del);
           glEnd();
       }glPopMatrix();
    }

    glColor3f(1.0f, 1.0f, 0.0f);         // Clear the color buffer (background)


    //circumcenter show

   for(int i=0;circumcenterShow && i<delaunay[0].size();i++)
	{
	    glPushMatrix();{

	        glTranslatef(norm(circumcenter[i].x),norm(circumcenter[i].y),0);

            glBegin(GL_QUADS);              // Each set of 4 vertices form a quad
                  // Red
                  glVertex2f(-del, -del);    // x, y
                  glVertex2f( del, -del);
                  glVertex2f( del,  del);
                  glVertex2f(-del,  del);
           glEnd();
       }glPopMatrix();
    }


//Traiangle show
    glColor3f(0.0f, 1.0f, 0.0f);



    for(int i=0;delaunayShow && i<delaunay[0].size();i++)
	{
        glBegin(GL_LINE_LOOP);

                glVertex2f(norm(input[delaunay[0][i]].x), norm(input[delaunay[0][i]].y));
                glVertex2f(norm(input[delaunay[1][i]].x), norm(input[delaunay[1][i]].y));
                glVertex2f(norm(input[delaunay[2][i]].x), norm(input[delaunay[2][i]].y));
        glEnd();
    }


    //Voronoi Diagram

    glColor3f(1.0f, 0.0f, 1.0f);

    for(int i=0;i<delaunay[0].size();i++)
	{
	    int a,b,c;
	    a=delaunay[0][i];
	    b=delaunay[1][i];
	    c=delaunay[2][i];

	    for(int j=i+1;j<delaunay[0].size();j++)
	    {
	        if(match(a,b,c,delaunay[0][j],delaunay[1][j],delaunay[2][j])==2)
	        {
                glBegin(GL_LINES);
                        glVertex2f(norm(circumcenter[i].x), norm(circumcenter[i].y));
                        glVertex2f(norm(circumcenter[j].x), norm(circumcenter[j].y));

                glEnd();
	        }
	    }



	    if(!valid(circumcenter[i])) continue;

	    if(tri_count[a][b]==1) draw_line_unbounded(input[a],input[b],circumcenter[i],input[c]);
	    if(tri_count[b][c]==1) draw_line_unbounded(input[b],input[c],circumcenter[i],input[a]);
	    if(tri_count[a][c]==1) draw_line_unbounded(input[a],input[c],circumcenter[i],input[b]);



    }


   glFlush();  // Render now
}

bool used[500][500];

void init(int sz){

    n = sz;

    for(int i=0;i<sz;i++){

        int x=rand()%400+10;
        int y=rand()%400+10;


        if(used[x][y])
        {
            i--;
            continue;
        }

        used[x][y]=1;

        input.push_back(point(x,y));

    }

    printf("%d\n",n);

    //for(int i=0;i<n;i++) printf("%.0lf %.0lf\n",input[i].x,input[i].y);
    freopen("C:\\Users\\Ashikee Abhi\\Google Drive\\input_voronoi.txt","w",stdout);
    for(int i=0; i< input.size();i++)
        cout<<input[i].x<< " "<<input[i].y<<endl;
}


void input_from_file()
{
    freopen("C:\\Users\\Ashikee Abhi\\Google Drive\\input_voronoi.txt","r",stdin);
    cin>>n;

    for(int i=0; i<n;i++)
    {
        double p,q;
        cin>>p>>q;
        point s(p,q);
        input.push_back(s);
    }
    for(int i=0; i< input.size();i++)
        cout<<input[i].x<< " "<<input[i].y<<endl;

}


void DelaunayTriangleCompute()
{
    int i,j,k,m,cnt=0;

    for(i = 0;i < n-2;i++)
    {
        double xi,yi,zi;
        xi = input[i].x;
        yi = input[i].y;
        zi = xi*xi+yi*yi;

        for(j=i+1;j<n;j++)
        {
            double xj,yj,zj;
            xj = input[j].x;
            yj = input[j].y;
            zj = xj*xj+yj*yj;

            for(k=i+1;k<n;k++)
            {
                if(k==j) continue;

                double xk,yk,zk,xn,yn,zn;
                xk = input[k].x;
                yk = input[k].y;
                zk = xk*xk+yk*yk;

                xn=(yj-yi)*(zk-zi) - (yk-yi)*(zj-zi);
                yn=(xk-xi)*(zj-zi) - (xj-xi)*(zk-zi);
                zn=(xj-xi)*(yk-yi) - (xk-xi)*(yj-yi);

                if(zn>=0) continue;

                for(m=0;m<n;m++)
                {
                    double xm,ym,zm;
                    xm = input[m].x;
                    ym = input[m].y;
                    zm = xm*xm+ym*ym;

                    if( (xm-xi)*xn + (ym-yi)*yn + (zm-zi)*zn  >0) break;
                }

                if(m==n)
                {
                    delaunay[0].push_back(i);
                    delaunay[1].push_back(j);
                    delaunay[2].push_back(k);
                    circumcenter.push_back(ComputeCircleCenter(input[i],input[j],input[k]));
                    cnt++;
                    tri_count[i][j]++;
                    tri_count[i][k]++;
                    tri_count[j][i]++;
                    tri_count[j][k]++;
                    tri_count[k][i]++;
                    tri_count[k][j]++;
                }
            }
        }
    }

    printf("%d\n",cnt);

}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {

   glutInit(&argc, argv);                 // Initialize GLUT
   glutCreateWindow("OpenGL Setup Test");
   //cin>>n;

   //init(n);
   input_from_file();
   DelaunayTriangleCompute();
    // Create a window with the given title
   glutInitWindowSize(500, 500);   // Set the window's initial width & height
   glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
   glutDisplayFunc(display); // Register display callback handler for window re-paint
   glutMainLoop();           // Enter the event-processing loop
   return 0;
}
