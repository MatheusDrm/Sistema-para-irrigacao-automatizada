# Sistema para Irrigação Automatizada

## Objetivo
Projeto que busca colocar em prática conhecimentos adquiridos na disciplina de sistemas embarcados, ao implementar um algoritmo utilizando FreeRTOS capaz de controlar uma válvula solenoide em função da umidade medida por sensores.

## Implementação

Este projeto também buscou comparar diferentes abordagens para resolver o problema discutido. 

### ESP32
O código feito utilizando FreeRtos, que terá suas funções sendo executadas concorrentemente, se encontra no arquivo irrigacaoAutomatizada_concorrente.ino.

![alt text](https://github.com/MatheusDrm/Sistema-para-irrigacao-automatizada/blob/main/diagramas/diagramacodigo_Concorrente.png)

Já o código que utiliza a estratégia sequencial do Super Loop se encontra em irrigacaoAutomatizada_sequencial.ino.

![alt text](https://github.com/MatheusDrm/Sistema-para-irrigacao-automatizada/blob/main/diagramas/diagrama_sequencial.png)

Por fim, para possibilitar a visualização da ordem de execução das tarefas alterou-se o código concorrente para obter o diagrama de GANTT (irrigacaoAutomatizada_concorrente_gantt.ino e gantt.py).

### Beagle bone
O código desenvolvido para essa placa se encontra em sequential_irrigation.py
