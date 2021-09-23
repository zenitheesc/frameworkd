#include "./path-handler.hpp"


PathHandler::parsedPath PathHandler::parsePath(std::string path){
    std::replace(path.begin(), path.end(), '/', ' ');
    std::vector<std::string> array;
    std::stringstream ss(path);

    std::string word;
    while (ss >> word) array.push_back(word);

    PathHandler::parsedPath parsedResponse;

    parsedResponse.service = array[0] + "." + array[1] + "." + array[2];
    parsedResponse.objectPath = "/" + array[0] + "/" + array[1] + "/" + array[2] + "/" + array[3];
    parsedResponse.interface = array[0] + "." + array[1] + "." + array[2] +"." + array[3];
    parsedResponse.functionality = array[4];

    return parsedResponse;
}

std::vector<std::string> PathHandler::splitPath(std::string path){
    std::replace(path.begin(), path.end(), '/', ' ');
    std::vector<std::string> parsedPath;
    std::stringstream ss(path);
    
    std::string word;
    while (ss >> word) parsedPath.push_back(word);

    return parsedPath;
}

bool PathHandler::regexMatch(std::string path) {
}

/*

Daemons do mesmo framework podem ser referenciadas do seguinte modo:

daemon/caminho/do/objeto:interface/funcionalidade

daemon/:interface/funcionalidade

nesse caso temos:

nome do servico: org.frameworkd.daemon

objectPath: org/frameworkd/daemon/caminho/do/objeto

interface: org.frameworkd.daemon.interface

obs: caso a interface tenha mais de um nome, o nome dela será todo o conteudo que estiver entre ::

nome da funcionalidade (sinal ou metodo ou propriedade (?)): funcionalidade


para falar com daemons do lado de fora do framework podera utilizar a mesma estrutura com os seguintes poren

nome.do.servico/caminho/do/objeto/nome.da.interface:metodo


TODO:

Achar um jeito de indicar que o nome da interface/ objeto deve ser concatenado com o nome do servico

nome.do.servico/caminho/do/objeto/nome.da.interface:funcionalidade

dbus.register("feature.cand/can/sender/sender:send",algumaCoisa, [](){

}, requirements["asdqwe"]);

{
    "asdasd": asdasd
    "asdasd":{
        "asdasd":{

        }
    }
}



*/
