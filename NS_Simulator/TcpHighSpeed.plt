set terminal png
set output "TcpHighSpeed.png"
set title "Congestion Window Calculation for TcpHighSpeed"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "TcpHighSpeed.cwnd" using 1:2 with linespoints title "Old Congestion","TcpHighSpeed.cwnd" using 1:3 with linespoints title "New Congestion"
