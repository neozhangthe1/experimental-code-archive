#
# Graph: ../as20graph.txt -> as20graph as20graph. A=0.001, K=10-1.5-100M, Cvr=10, SzFrc=0.001 G(6474, 13233) (Wed Jun 08 08:52:26 2011)
#

set title "Graph: ../as20graph.txt -> as20graph as20graph. A=0.001, K=10-1.5-100M, Cvr=10, SzFrc=0.001 G(6474, 13233)"
set key bottom right
set logscale xy 10
set format x "10^{%L}"
set mxtics 10
set format y "10^{%L}"
set mytics 10
set grid
set xlabel "k (number of nodes in the cluster)"
set ylabel "{/Symbol \F} (conductance)"
set ticscale 2 1
set terminal png small size 1000,800
set output 'ncp.as20graph.png'
plot 	"ncp.as20graph.tab" using 1:2 title "ORIGINAL MIN (6474, 13233)" with lines lw 1
