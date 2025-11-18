<img width="820" height="735" alt="image" src="https://github.com/user-attachments/assets/0ee0c9b4-60f5-4ddf-b2f2-22666c93bd10" />

* código utilizado:
1. shape: sequence_diagram
2. Main -> Fila_de_mensagens: k_msgq_get(K_FOREVER)
3. Usuario -> UART_Hardware: Hello!
4. UART_Hardware -> UART_Driver: interrupção
5. UART_Driver -> Fila_de_mensagens: k_msgq_put("Hello!")
6. Fila_de_mensagens -> Main: Hello!
7. Main -> UART_Hardware: Echo: Hello!
8. UART_Hardware -> Usuario: Echo: Hello!
