/*
 * Versão: RX nunca é desabilitado no driver.
 * A ISR sempre esvazia o FIFO; quando rx_app_enabled == false,
 * os bytes são apenas descartados (contados).
 *
 * Mantive a lógica original: TX por polling, alternância periódica
 * (habilita/desabilita rx lógico).
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <stdbool.h>

#define PERIODO_SEGUNDOS 5
#define LOOP_ITER_MAX_TX 4
#define MAX_TX_LEN 64

#define UART_DEVICE_NODE DT_NODELABEL(uart0)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

#define RX_BUF_SIZE 64
static char rx_buf[RX_BUF_SIZE];
static volatile int rx_buf_pos = 0;

/* flag lógica: quando true, a aplicação processa bytes; quando false, ISR descarta */
static volatile bool rx_app_enabled = false;

/* contador de bytes descartados enquanto rx_app_enabled == false (útil p/ debug) */
static volatile unsigned int dropped_bytes = 0;

static char tx_buffer[MAX_TX_LEN];

/* TX por polling (bloqueante) */
static void print_uart_poll(const char *buf)
{
    int msg_len = strlen(buf);
    for (int i = 0; i < msg_len; i++) {
        uart_poll_out(uart_dev, buf[i]);
    }
}

/* ISR: sempre lê tudo do FIFO; processa só se rx_app_enabled for true */
static void uart_isr(const struct device *dev, void *user_data)
{
    ARG_UNUSED(user_data);

    /* Nota: mantemos a mesma estrutura: while(uart_irq_update && uart_irq_rx_ready) */
    while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) {
        uint8_t c;
        /* lê enquanto houver bytes no FIFO */
        while (uart_fifo_read(dev, &c, 1) == 1) {
            if (!rx_app_enabled) {
                /* descartamos bytes recebidos fora do período — incrementa contador para debug */
                dropped_bytes++;
                /* opcional: poderíamos limitar o contador para evitar overflow */
            } else {
                /* Processamento normal: monta linha até CR/LF */
                if ((c == '\n' || c == '\r') && rx_buf_pos > 0) {
                    rx_buf[rx_buf_pos] = '\0';
                    printk("RX (ISR): %s\n", rx_buf);
                    rx_buf_pos = 0;
                } else if (rx_buf_pos < (RX_BUF_SIZE - 1)) {
                    rx_buf[rx_buf_pos++] = c;
                } else {
                    /* buffer cheio: descarta e reseta (evita travamento pelo overflow) */
                    rx_buf_pos = 0;
                    /* opcional: sinalizar overflow */
                    printk("RX buffer overflow - descartando dados\n");
                }
            }
        }
    }
}

/* Habilita/desabilita o RX da "aplicação" com proteção (irq_lock) */
static void set_rx_app_enabled(bool enable)
{
    unsigned int key = irq_lock();
    if (enable) {
        /* limpa estado da aplicação: zera buffer e contador de descartes */
        rx_buf_pos = 0;
        dropped_bytes = 0;
        rx_app_enabled = true;
    } else {
        rx_app_enabled = false;
        /* manter dropped_bytes acumulando para diagnóstico */
    }
    irq_unlock(key);
}

void main(void)
{
    int loop_counter = 0;

    if (!device_is_ready(uart_dev)) {
        printk("Erro: UART0 não está pronta!\n");
        return;
    }

    uart_irq_callback_user_data_set(uart_dev, uart_isr, NULL);
    uart_irq_rx_enable(uart_dev);

    printk("App iniciada. RX lógico alterna, mas RX HW fica sempre habilitado.\n");
    printk("TX por polling; printk e app compartilham UART0.\n");

    /* timestamp para medir delta-t entre loops */
    uint64_t last_ts = k_uptime_get();

    while (1) {

        /* --- cálculo do delta-t (antes do sleep) --- */
        uint64_t now = k_uptime_get();
        uint64_t delta_ms = now - last_ts;
        last_ts = now;

        printk("\n---- Loop %d (Δt = %llu ms) ----\n", loop_counter, delta_ms);
        /* agora dorme o período nominal */
        k_sleep(K_SECONDS(PERIODO_SEGUNDOS));

        /* --- TX (síncrono) --- */
        int num_tx = (loop_counter % LOOP_ITER_MAX_TX) + 1;
        printk("Enviando %d pacotes (Polling TX)...\n", num_tx);

        for (int i = 0; i < num_tx; i++) {
            snprintk(tx_buffer, MAX_TX_LEN, "[Placa %d] Pacote %d\r\n",
                     loop_counter, i);
            print_uart_poll(tx_buffer);
        }

        /* --- Alterna RX lógico --- */
        bool new_rx_state = !rx_app_enabled;
        set_rx_app_enabled(new_rx_state);

        if (new_rx_state) {
            printk("RX (aplicação) [HABILITADO]\n");
        } else {
            printk("RX (aplicação) [DESABILITADO]\n");
            if (dropped_bytes) {
                printk("  (bytes descartados enquanto desabilitado: %u)\n",
                       dropped_bytes);
            }
        }

        loop_counter++;
    }
}


