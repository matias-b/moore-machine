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

    // Agregar un estado a Q y su simbolo de salida ℎ:𝑄 → 𝛤
    void addState(const std::string &name, const std::string &outputName)
    {

        if (outputFunctions.find(outputName) == outputFunctions.end())
        {
            throw "Función de salida no registrada: " + outputName;
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

    // Agregar una transición, si input es null se vuelve una transicion lambda
    void addTransition(const std::string &fromState, const std::string &input, const std::string &toState )
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
        }
        else
        {
            // Contemplar transiciones lambda
            if (input.empty())
            {
                std::cout << states[fromIndex].name << " -- LAMBDA -->" << states[toIndex].name << std::endl;
                states[fromIndex].transitions["_LAMBDA"] = toIndex;
            }
            else
            {
                std::cout << states[fromIndex].name << " -- " << input << "-->" << states[toIndex].name << std::endl;
                states[fromIndex].transitions[input] = toIndex;
            }
        }
    }

    // Reiniciar la máquina
    void reset(bool executeStateOutput = true, bool showInputsOutputs = true)
    {
        std::cout << "----Reiniciando máquina----" << std::endl;
        if (initialState == -1)
            std::cout << "Estado inicial no establecido";
        currentState = initialState;

        if (showInputsOutputs)
        {
            std::cout << "Estado inicial: " << states[currentState].name << std::endl;

            std::cout << "Salidas: ";
            for (auto &output : outputFunctions)
            {
                std::cout << output.first << " ";
            }
        }
        if (executeStateOutput)
            executeOutput();
    }

    // Procesar una entrada
    void processInput(const std::string &input, bool executeStateOutput = true)
    {
        if (currentState == -1)
            throw "La máquina no está inicializada";

        auto it = states[currentState].transitions.find(input);
        if (it != states[currentState].transitions.end())
        {
            currentState = it->second;

            if (executeStateOutput)
                executeOutput();

            // Ejecutar transiciones lambda
            processInput("_LAMBDA", executeStateOutput);
        }
        else
        {
            // Ignorar error si la entrada es lambda
            if (input != "_LAMBDA")
                 std::cout << "Transición no encontrada para la entrada dada" << std::endl;
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
            throw "La máquina no está inicializada";

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
            throw "La máquina no está inicializada";
        return states[currentState].name;
    }
};
