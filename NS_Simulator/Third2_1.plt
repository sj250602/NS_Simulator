set terminal png
set output "Third2.png"
set title "Congestion Window Calculation for TcpNewReno"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "Third2.cwnd" using 1:2 with linespoints title "Old Congestion","Third2.cwnd" using 1:3 with linespoints title "New Congestion"