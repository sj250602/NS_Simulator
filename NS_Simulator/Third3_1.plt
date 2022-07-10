set terminal png
set output "Third3.png"
set title "Congestion Window Calculation for TcpNewReno"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "Third3.cwnd" using 1:2 with linespoints title "Old Congestion","Third3.cwnd" using 1:3 with linespoints title "New Congestion"