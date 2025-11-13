   <img width="1826" height="1202" alt="image" src="https://github.com/user-attachments/assets/de206f01-7de4-4d2e-b68a-2fbe67d4b135" />

1. main -> UART_Driver: uart_callback_set(uart_cb)
2. main -> UART_Driver: inicializa RX como 0
3. main -> sem: inicializa semáforo como 1
4, main -> sem: Pega semáforo
5. main -> UART_Driver: acessa buffer
6. UART_Driver -> UART0: Inicializa transmissão assíncrona
7. UART0 -> UART_Driver: Completa transmissão
8. UART_Driver -> sem: Libera semáforo
9. main -> UART_Driver: alterna RX
10. UART0 -> UART_Driver: Recebe bytes
11. UART_Driver -> App: printk("RX: ....")

