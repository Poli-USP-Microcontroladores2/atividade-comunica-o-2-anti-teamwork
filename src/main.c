/*
 * UART0 async TX/RX example for FRDM-KL25Z
 * Compatible with Zephyr 4.2 and minimal headers
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define UART_DEVICE_NODE DT_NODELABEL(uart0)

/* Configurações */
#define LOOP_ITER_MAX_TX 4
#define MAX_TX_LEN 64
#define RX_CHUNK_LEN 32

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

/* Buffers de TX e RX */
static char tx_buffer[MAX_TX_LEN];
static uint8_t rx_buffers[2][RX_CHUNK_LEN];
static volatile uint8_t rx_index = 0;

/* Controle de estado */
static bool rx_enabled = false;
static struct k_sem tx_done_sem;

/* Callback de eventos UART */
static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    switch (evt->type) {
    case UART_TX_DONE:
        /* Libera semáforo para permitir novo envio */
        k_sem_give(&tx_done_sem);
        break;

    case UART_RX_BUF_REQUEST:
        /* Fornece o próximo buffer de recepção */
        rx_index ^= 1; /* alterna entre 0 e 1 */
        uart_rx_buf_rsp(dev, rx_buffers[rx_index], sizeof(rx_buffers[0]));
        break;

    case UART_RX_RDY:
        /* Dados recebidos */
        printk("RX: %.*s\n", evt->data.rx.len, evt->data.rx.buf + evt->data.rx.offset);
        break;

    case UART_RX_DISABLED:
        printk("UART RX desabilitado\n");
        break;

    default:
        break;
    }
}

void main(void)
{
    int loop_counter = 0;

    if (!device_is_ready(uart_dev)) {
        printk("Erro: UART0 não está pronta!\n");
        return;
    }

    k_sem_init(&tx_done_sem, 1, 1);
    uart_callback_set(uart_dev, uart_cb, NULL);

    printk("Iniciando exemplo UART0 assíncrona (FRDM-KL25Z)\n");

    while (1) {
        k_sleep(K_SECONDS(5));

        int num_tx = (loop_counter % LOOP_ITER_MAX_TX) + 1;
        printk("\nLoop %d: enviando %d pacotes...\n", loop_counter, num_tx);

        for (int i = 0; i < num_tx; i++) {
            /* Espera TX anterior terminar */
            k_sem_take(&tx_done_sem, K_FOREVER);

            int len = snprintk(tx_buffer, sizeof(tx_buffer),
                               "Loop %d, Pacote %d\r\n", loop_counter, i);

            int rc = uart_tx(uart_dev, tx_buffer, len, SYS_FOREVER_US);
            if (rc != 0) {
                printk("Erro no uart_tx (%d)\n", rc);
                k_sem_give(&tx_done_sem);
            }
        }

        /* Alterna o estado do RX */
        if (rx_enabled) {
            uart_rx_disable(uart_dev);
        } else {
            rx_index = 0;
            uart_rx_enable(uart_dev, rx_buffers[0], RX_CHUNK_LEN, 100);
        }

        rx_enabled = !rx_enabled;
        printk("RX agora está %s\n", rx_enabled ? "habilitado" : "desabilitado");

        loop_counter++;
    }
}
