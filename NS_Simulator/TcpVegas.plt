set terminal png
set output "TcpVegas.png"
set title "Congestion Window Calculation for TcpVegas"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "TcpVegas.cwnd" using 1:2 with linespoints title "Old Congestion","TcpVegas.cwnd" using 1:3 with linespoints title "New Congestion"
