set terminal png
set output "TcpVeno.png"
set title "Congestion Window Calculation for TcpVeno"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "TcpVeno.cwnd" using 1:2 with linespoints title "Old Congestion","TcpVeno.cwnd" using 1:3 with linespoints title "New Congestion"
