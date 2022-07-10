set terminal png
set output "TcpNewReno.png"
set title "Congestion Window Calculation for TCPNewReno"
set xlabel "Time (in Seconds)"
set ylabel "Congestion Window Size (cwnd)"
plot "TcpNewReno.cwnd" using 1:2 with linespoints title "Old Congestion","TcpNewReno.cwnd" using 1:3 with linespoints title "New Congestion"
