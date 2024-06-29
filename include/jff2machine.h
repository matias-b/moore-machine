#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "tinyxml2.h"
#include "MooreMachine.h"

using namespace tinyxml2;

// Función para leer el archivo .jff y configurar la máquina de Moore
void loadMooreMachineFromJFF(const std::string &filename, MooreMachine &machine)
{
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS)
    {
        throw "Error al cargar el archivo JFF";
    }

    XMLElement *root = doc.RootElement();
    if (std::string(root->FirstChildElement("type")->GetText()) != "moore")
    {
        throw "El archivo no contiene una máquina de Moore";
    }

    XMLElement *automaton = root->FirstChildElement("automaton");

    // Mapas para traducir IDs de estados a nombres y salidas
    std::map<std::string, std::string> stateNames;

    // Leer estados
    for (XMLElement *state = automaton->FirstChildElement("state"); state != nullptr; state = state->NextSiblingElement("state"))
    {
        std::string id = state->Attribute("id");
        std::string name = state->Attribute("name");
        std::string output = state->FirstChildElement("output")->GetText();

        stateNames[id] = name;

        // Agregar el estado con el nombre de la salida
        machine.addState(name, output);

        if (state->FirstChildElement("initial"))
        {
            machine.setInitialState(name);
        }
    }

    // Leer transiciones
    for (XMLElement *transition = automaton->FirstChildElement("transition"); transition != nullptr; transition = transition->NextSiblingElement("transition"))
    {
        std::string from = transition->FirstChildElement("from")->GetText();
        std::string to = transition->FirstChildElement("to")->GetText();
        std::string read = transition->FirstChildElement("read")->GetText();

        machine.addTransition(stateNames[from], read, stateNames[to]);
    }
}

// Función para leer el archivo .jff y configurar la máquina de Moore
void loadMooreMachineFromJFFString(const std::string &content, MooreMachine &machine)
{
    XMLDocument doc;
    if (doc.Parse(content.c_str()) != XML_SUCCESS)
    {
        std::cout << "Error al cargar el string JFF" << std::endl;
        return;
        // throw std::exception("Error al cargar el string JFF");
    }

    XMLElement *root = doc.RootElement();
    if (std::string(root->FirstChildElement("type")->GetText()) != "moore")
    {
        throw "El archivo no contiene una máquina de Moore";
    }

    XMLElement *automaton = root->FirstChildElement("automaton");

    // Mapas para traducir IDs de estados a nombres y salidas
    std::map<std::string, std::string> stateNames;

    // Limpiar estados
    machine.clearStates();

    // Leer estados
    for (XMLElement *state = automaton->FirstChildElement("state"); state != nullptr; state = state->NextSiblingElement("state"))
    {
        std::string id = state->Attribute("id");
        std::string name = state->Attribute("name");
        std::string output = state->FirstChildElement("output")->GetText();

        std::cout << "Estado: " << name << " -> " << output << std::endl;
        stateNames[id] = name;

        // Agregar el estado con el nombre de la salida
        machine.addState(name, output);

        if (state->FirstChildElement("initial"))
        {
            machine.setInitialState(name);
        }
    }

    // Leer transiciones
    for (XMLElement *transition = automaton->FirstChildElement("transition"); transition != nullptr; transition = transition->NextSiblingElement("transition"))
    {
        std::string from = transition->FirstChildElement("from")->GetText();
        std::string to = transition->FirstChildElement("to")->GetText();

        XMLElement *readElement = transition->FirstChildElement("read");
        std::string read;

        if (readElement && readElement->GetText())
            read = readElement->GetText();

        // if(read == nullptr)
        // read = "_LAMBDA";
        //   std::cout << "Transición: " << from << " -> " << read << " -> " << to << std::endl;

        machine.addTransition(stateNames[from], read, stateNames[to]);
    }

    machine.reset();

    std::cout << "Máquina de Moore cargada" << std::endl;
}