set terminal png
set output "Second.png"
set title "Congestion Window Calculation for TcpNewReno"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "Second.cwnd" using 1:2 with linespoints title "Old Congestion","Second.cwnd" using 1:3 with linespoints title "New Congestion"
