

typedef struct stats {
	unsigned int	rx_packets;
	unsigned int    rx_errs;
	unsigned int    rx_drop;
	unsigned int    rx_fifo;
	unsigned int    rx_frame;

        unsigned int    tx_packets;
        unsigned int    tx_errs;
        unsigned int    tx_drop;
        unsigned int    tx_fifo;
        unsigned int    tx_colls;
} stats_t;


