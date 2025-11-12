# PSI-Microcontroladores2-Aula10
Atividade: Comunicação UART

# Projeto UART – Atividade em Duplas (Echo Bot + Async API)

* Dupla:

  * Integrante 1: Guilherme da Silva Fernandes
  * Integrante 2: Bruno Mora

## 3.1 Descrição do Funcionamento

* O programa é capaz de receber mensagens pelo serial monitor e repetir elas pela porta serial através do serial monitor como se o que foi escrito tivesse ecoado. É importante salientar que ele é capaz de repetir até 511 caractéres por mensagem, caractéres acentuádos valem por 2 e espaçamentos são contabilizados.
* Ele utiliza da interface UART(serial) para receber as linhas do usuário e seu funcionamento é graças as interrupções que são geradas toda vez que um caractere novo entra na porta UART. Esses caracteres são armazenados em um buffer ao detectar o enter(\r) do usuário e colocados na fila. O main acorda quando vê essas mensagens, lê elas e envia de volta como um eco.

## 3.2 Casos de Teste Planejados (TDD)

### CT1 – Eco básico
* Entrada: Hello
* Saída esperada: Hello
* Critério de Aceitação: imprimir "Echo:Hello"

### CT2 – Linha vazia
* Entrada: vazia
* Saída esperada: Vazio
* critério de aceitação: não imprimir nada
  
### CT3 – Linha longa
* Entrada: três pratos de trigo para três tigres tristes
* Saída esperada: três pratos de trigo para três 
* Critério de aceitação: imprimir toda a frase

### CT4 - Números:
* Entrada: 1234567899
* Saida: 123456789
* Critério de aceitação: ser capaz de imprimir esses números

### CT5 - Caractéres especiais:
* Entrada: !@#$%¨&*()+=-_{}^<>?[]
* Saída esperada: imprimir todos os caractéres enviados.
* Critério de aceitação: conseguir ecoar os caracteres

## 3.3 Implementação

* Arquivo(s) modificados: prj conf teve sua estrutura alterada para adicionar as linhas: "CONFIG_SERIAL=y" e "CONFIG_UART_INTERRUPT_DRIVEN=y". Além disso, o main.c teve o número de caractéres alterado de 32 para 512.
* Justificativa das alterações: Essas alterações foram necessárias para habilitar a porta serial e para tornar a resposta mais eficiente e veloz, pois permite que a cpu não precise verificar a todo momento se o UART está liberado. Além disso, as mudanças do main permitem que o echo bot ecoe mais caractéres do que originalmente, visto que ele era capaz de imprimir 31 e agora consegue 512.
* 
## 3.4 Evidências de Funcionamento

* 1° Eco básico:

<img width="1334" height="109" alt="image" src="https://github.com/user-attachments/assets/88d9cf0a-ac5f-49b1-981b-591c961b9a52" />

* 2° Texto vazio:
  
<img width="1334" height="109" alt="image" src="https://github.com/user-attachments/assets/6a46cb2f-7951-4192-a131-7bc673c58cda" />

* 3° Texto longo:

<img width="1334" height="109" alt="image" src="https://github.com/user-attachments/assets/595cb5e0-4333-4f0d-9a08-69116365c5b4" />

* 4° Números:

<img width="1334" height="109" alt="image" src="https://github.com/user-attachments/assets/af1fe980-c04c-4185-8e13-34541187973f" />

* 5° Caractéres especiais:

<img width="1334" height="109" alt="image" src="https://github.com/user-attachments/assets/1b1d987d-fb87-406d-8424-8038b2199c10" />

## Logs:

- CT1:
  
1. Hello! I'm your echo bot.
2. Tell me something and press enter:
3. ---- Sent utf8 encoded message: "Hello\r" ----
4. Echo: Hello

- CT2:
  
1. Hello! I'm your echo bot.
2. Tell me something and press enter:
3. ---- Sent utf8 encoded message: "\r" ----

- CT3:
1. Hello! I'm your echo bot.
2. Tell me something and press enter:  
3. ---- Sent utf8 encoded message: "Três pratos de trigo para três tigres tristes\r" ----
4. Echo: Três pratos de trigo para três tigres tristes

- CT4:

1. Hello! I'm your echo bot.
2. Tell me something and press enter:
3. ---- Sent utf8 encoded message: "123456789\r" ----
4. Echo: 123456789

- CT5:
  
1. Hello! I'm your echo bot.
2. Tell me something and press enter:
3. ---- Sent utf8 encoded message: "!@#$%&*()+=-_{}^<>?[]\r" ----
4. Echo: !@#$%&*()+=-_{}^<>?[]

## 3.5 Diagramas de Sequência D2:

<img width="820" height="735" alt="image" src="https://github.com/user-attachments/assets/0ee0c9b4-60f5-4ddf-b2f2-22666c93bd10" />

* código utilizado:
shape: sequence_diagram
Main -> Fila_de_mensagens: k_msgq_get(K_FOREVER)
Usuario -> UART_Hardware: Hello!
UART_Hardware -> UART_Driver: interrupção
UART_Driver -> Fila_de_mensagens: k_msgq_put("Hello!")
Fila_de_mensagens -> Main: Hello!
Main -> UART_Hardware: Echo: Hello!
UART_Hardware -> Usuario: Echo: Hello!
# 4. Etapa 2 – Async API (Transmissão/Recepção Assíncrona)

## 4.1 Descrição do Funcionamento

É esperado que a cada 5 segundos ele envie pacotes de texto entre 1 e 4, em loop sem parar e sem problemas relacionados a interrupções. 

## 4.2 Casos de Teste Planejados (TDD)

### CT1 – Transmissão de pacotes a cada 5s
*
### CT2 – Recepção
*
### CT3 – Verificação de timing dos 5s
*

## 4.3 Implementação

* Arquivos modificados:
* Motivos/Justificativas:

## 4.4 Evidências de Funcionamento

Salvar em `docs/evidence/async_api/`.

Exemplo:

```
Loop 0:
Sending 3 packets (packet size: 5)
Packet: 0
Packet: 1
Packet: 2
```

Ou:

```
RX is now enabled
UART callback: RX_RDY
Data (HEX): 48 65 6C 6C 6F
Data (ASCII): Hello
```

## 4.5 Diagramas de Sequência D2

Vide material de referência: https://d2lang.com/tour/sequence-diagrams/

Adicionar arquivos (diagrama completo e o código-base para geração do diagrama) em `docs/sequence-diagrams/`.

---

# 5. Conclusões da Dupla

* O que deu certo:
* O que foi mais desafiador:
