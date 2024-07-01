#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <functional>

// Definición de la clase MooreMachine
class MooreMachine
{
private:
    // Estructura para representar un estado
    struct State
    {
        std::string name;
        std::function<void()> output;
        std::map<std::string, int> transitions; // mapa de transiciones: entrada -> índice del siguiente estado
    };

    std::vector<State> states;                                    // vector de estados
    std::map<std::string, std::function<void()>> outputFunctions; // mapa de nombres de salida a funciones
    int initialState;                                             // índice del estado inicial
    int currentState;                                             // índice del estado actual

public:
    // Constructor
    MooreMachine() : initialState(-1), currentState(-1) {}

    // Registrar una función de salida (𝛤 → función)
    void addOutput(const std::string &outputName, std::function<void()> outputFunction)
    {
        outputFunctions[outputName] = outputFunction;
    }

    // Agregar un estado a Q y su simbolo de salida, ℎ:𝑄 → 𝛤
    void addState(const std::string &name, const std::string &outputName)
    {

        if (outputFunctions.find(outputName) == outputFunctions.end())
        {
            std::cout << "Función de salida no registrada: " + outputName << std::endl;
            return;
        }
        states.push_back({name, outputFunctions[outputName], {}});
    }

    // Establecer el estado inicial
    void setInitialState(const std::string &name)
    {
        for (size_t i = 0; i < states.size(); ++i)
        {
            if (states[i].name == name)
            {
                initialState = i;
                currentState = i;
                return;
            }
        }
        throw "Estado inicial no encontrado";
    }

    // Agregar una transición
    void addTransition(const std::string &fromState, const std::string &input, const std::string &toState)
    {
        int fromIndex = -1, toIndex = -1;

        for (size_t i = 0; i < states.size(); ++i)
        {
            if (states[i].name == fromState)
                fromIndex = i;
            if (states[i].name == toState)
                toIndex = i;
        }
        if (fromIndex == -1 || toIndex == -1)
        {
            // throw std::exception("Estados no encontrados para la transición");
            std::cout << "Estados no encontrados para la transición" << std::endl;
            return;
        }
        else
        {
            // Si la entrada es vacia, se asume que es tipo OUT
            if (input.empty())
            {
                std::cout << states[fromIndex].name << " -- _out -->" << states[toIndex].name << std::endl;
                states[fromIndex].transitions["_out"] = toIndex;
            }
            else
            {
                std::cout << states[fromIndex].name << " -- " << input << " -->" << states[toIndex].name << std::endl;
                states[fromIndex].transitions[input] = toIndex;
            }
        }
    }

    // Reiniciar la máquina
    void reset(bool executeStateOutput = true, bool showInputsOutputs = true)
    {
        std::cout << "----Reiniciando máquina----" << std::endl;
        if (initialState == -1)
        {
            std::cout << "Estado inicial no establecido";
            return;
        }

        currentState = initialState;

        if (showInputsOutputs)
        {
            std::cout << "Estado inicial: " << states[currentState].name << std::endl;

            std::cout << "Salidas: ";
            for (auto &output : outputFunctions)
            {
                std::cout << output.first << " ";
            }
            std::cout << std::endl;
        }

        if (executeStateOutput)
        {
            executeOutput();
        } 
        processInput("_out", executeStateOutput);
    }

    // Procesar una entrada
    void processInput(const std::string &input, bool executeStateOutput = true)
    {
        if (currentState == -1)
        {
            std::cout << "La máquina no está inicializada" << std::endl;
            return;
        }

        auto it = states[currentState].transitions.find(input);
        if (it != states[currentState].transitions.end())
        {
            // Mostrar transicion
            std::cout << states[currentState].name << " -- " << input << " --> " << states[it->second].name << std::endl;

            currentState = it->second;
            if (executeStateOutput)
                executeOutput();

            // Ejecutar transiciones OUT
            processInput("_out", executeStateOutput);
        }
        else
        {
            // Ignorar error si la entrada es OUT
            if (input != "_out")
            {
                // std::cout << "Transición no encontrada para la entrada dada" << std::endl;
                return;
            }
        }
    }

    // Limpiar los estados
    void clearStates()
    {
        states.clear();
        initialState = -1;
        currentState = -1;
    }

    // Ejecutar la salida del estado actual
    void executeOutput() const
    {
        if (currentState == -1)
        {
            std::cout << "La máquina no está inicializada" << std::endl;
            return;
        }

        if (states[currentState].output != nullptr)
        {
            states[currentState].output();
        }
        else
        {
            std::cout << ("No hay salida definida para el estado " + states[currentState].name) << std::endl;
        }
    }

    // Obtener el nombre del estado actual
    std::string getCurrentState() const
    {
        if (currentState == -1)
        {
            std::cout << "La máquina no está inicializada" << std::endl;
            return "";
        }
        return states[currentState].name;
    }
};
