﻿#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <WinSock2.h>
#include <vector>
#include <thread>
#include <ctime>
#include <chrono>
#include <Ws2tcpip.h>
#include <format>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iterator>

const std::string SERVER_IP = "0.0.0.0";
//const char* key = generate_key();
int PORT = 5050;

int	ft_atoi(const char* a)
{
    int	ret;
    int	sign;

    ret = 0;
    sign = 1;
    while ((*a >= 9 && *a <= 13) || *a == 32)
    {
        a++;
    }
    if (*a == '+' || *a == '-')
    {
        if (*a == '-')
            sign = -1;
        a++;
    }
    while (*a >= '0' && *a <= '9')
    {
        ret = ret * 10 + (*a - '0');
        a++;
    }
    return (sign * ret);
}

class db
{
    public:
        db() {}

        db(std::string name, std::vector<std::string> initial_values)
            :name_(name), header(initial_values)
        {}

        void rename(std::string new_name)
        {
            name_ = new_name;
        }

        void add_value(std::vector<std::string> new_value)
        {
            values.push_back(new_value);
            size++;
        }
        
        void remove_value(int index)
        {
            values.erase(values.begin() + index);
        }
        
        std::vector<std::vector<std::string>> get_values()
        {
            return values;
        }

        std::vector<std::string> get_value(int index)
        {
            return values[index];
        }
        
        std::string get_name()
        {
            return name_;
        }

        int get_size()
        {
            return size;
        }

        int header_size()
        {
            return header.size();
        }
        std::vector<std::string> get_header()
        {
            return header;
        }

    private:
        std::string name_;
        std::vector<std::string> header;
        std::vector<std::vector<std::string>> values;
        int size = 0;
};
std::vector<db> dbs;

std::vector<std::string> tokenize(std::string result)
{
    std::vector<std::string> tokens;
    std::stringstream check1(result);

    std::string intermediate;
    while (getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
    return tokens;
}

void print_line(std::vector<std::string> header)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "+";
        std::cout << "-";
        for (unsigned int x = 0; x < header[i].size(); x++)
            std::cout << "-";
        std::cout << "-";
    }
    std::cout << "+";
    std::cout << std::endl;
}

void print_line(std::vector<std::string> header, std::vector<int> max)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "+";
        std::cout << "-";
        for (unsigned int x = 0; x < max[i]; x++)
            std::cout << "-";
        std::cout << "-";
    }
    std::cout << "+";
    std::cout << std::endl;
}

void print_words(std::vector<std::string> header)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "|" << " " << header[i] << " ";
    }
    std::cout << "|";
    std::cout << std::endl;
}

void print_words(std::vector<std::string> header, std::vector<int> max)
{
    for (unsigned int i = 0; i < header.size(); i++)
    {
        std::cout << "|" << " " << header[i];
        for (unsigned int x = 0; x < (max[i] - (header[i].length() - 1)); x++)
            std::cout << " ";
    }
    std::cout << "|";
    std::cout << std::endl;
}

void db_sv(SOCKET cliente)
{
    int bytes_read = 0;
    char* buffer = (char*)calloc(1024, sizeof(char));
    std::cout << "Yayy" << std::endl; //CREATE {name} {val1} {val2} {val∞}...
    while (1)
    {
        bytes_read = recv(cliente, buffer, 1024, 0);
        if (bytes_read == -1)
        {
            break;
        }
        std::string result = buffer;
        std::vector<std::string> tokens = tokenize(result);
        for (unsigned int i = 0; i < tokens.size(); i++)
        {
            std::cout << tokens[i] << std::endl;
        }
        memset(buffer, 0, 1024);
    }

}

void evaluate(std::vector<std::string> tokens)
{
    if (tokens[0].compare("create") == 0)
    {
        if (tokens.size() < 3)
        {
            std::cout << "Peticion de creacion invalida" << std::endl;
            return;
        }
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << "Una base de datos con este nombre ya ha sido creada" << std::endl;
                return;
            }
        }
        std::vector<std::string> buffer;
        std::string write_file;
        for (unsigned int i = 2; i < tokens.size(); i++)
        {
            buffer.push_back(tokens[i]);
            write_file.append(tokens[i]);
            if (i < tokens.size() - 1)
                write_file.append(" ");
        }
        dbs.push_back(db(tokens[1], buffer));
        std::ofstream db(tokens[1].append(".ldb"));
        db << write_file.append("\n");
        std::cout << "La base de datos se ha creado correctamente" << std::endl;
        db.close();
    }

    if (tokens[0].compare("add") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                if (tokens.size() - 2 == dbs[i].header_size())
                {
                    std::vector<std::string> db_push;
                    std::string db_write;
                    for (unsigned int i = 2; i < tokens.size(); i++)
                    {
                        db_push.push_back(tokens[i]);
                        db_write.append(tokens[i]);
                        if (i < tokens.size() - 1)
                            db_write.append(" ");
                    }
                    dbs[i].add_value(db_push);
                    std::ofstream db_writee(tokens[1].append(".ldb"), std::ios_base::app);
                    db_writee << db_write.append("\n");
                }
                else
                {
                    std::cout << "La cantidad de datos introducidos es incorrecta" << std::endl;
                }
                return;
            }
        }
        std::cout << "No se ha encontrado una base de datos con ese nombre (ls para ver las bases de datos activas)" << std::endl;
    }

    if (tokens[0].compare("show") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::vector<int> max;
                std::vector<std::vector<std::string>> values = dbs[i].get_values();
                std::vector<std::string> header = dbs[i].get_header();
                for (unsigned int y = 0; y < header.size(); y++)
                {
                    max.push_back(header[y].length());
                    for (unsigned int x = 0; x < values.size(); x++)
                    {
                        if (max.empty() || (max.size() - 1) < y)
                        {
                            max.push_back(values[x][y].size());
                            continue;
                        }
                        if (values[x][y].size() > max[y])
                            max[y] = values[x][y].size();
                    }
                }
                print_line(header, max);
                print_words(header, max);
                print_line(header, max);
                for (unsigned int y = 0; y < values.size(); y++)
                {
                    print_words(values[y], max);
                    print_line(values[y], max);
                }
                
                return;
            }
        }
    }

    if (tokens[0].compare("ls") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            std::cout << dbs[i].get_name();
            if (i < dbs.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;
    }
    if (tokens[0].compare("size") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << dbs[i].get_size() << std::endl;
                return;
            }
        }
        std::cout << "No se ha encontrado una base de datos con ese nombre (ls para ver las bases de datos activas)" << std::endl;
    }
    if (tokens[0].compare("load") == 0)
    {
        std::string name = tokens[1];
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                std::cout << "Ya existe una base de datos con ese nombre (remame para cambiar el nombre)" << std::endl;
                return;
            }
        }
        if (std::filesystem::exists(tokens[1].append(".ldb")))
        {
            std::ifstream infile(tokens[1]);
            std::string head;
            std::getline(infile, head);
            std::cout << head << std::endl;
            std::vector<std::string> header = tokenize(head);
            dbs.push_back(db(name, header));
            std::string linea;
            while (std::getline(infile, linea))
            {
                std::cout << linea << std::endl;
                std::vector<std::string> values = tokenize(linea);
                dbs.back().add_value(values);
                //linea.clear();
            }
            return;
        }
        std::cout << "El archivo con ese nombre no fue encontrado recuerda que tiene que tener la extension .ldb" << std::endl;
    }
    if (tokens[0].compare("rename") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0)
            {
                dbs[i].rename(tokens[2]);
                std::rename(tokens[1].append(".ldb").c_str(), tokens[2].append(".ldb").c_str());
            }
        }
    }
    if (tokens[0].compare("get") == 0)
    {
        for (unsigned int i = 0; i < dbs.size(); i++)
        {
            if (dbs[i].get_name().compare(tokens[1]) == 0 && dbs[i].get_size() > ft_atoi(tokens[2].c_str()) - 1)
            {
                if (tokens.size() > 2)
                {
                    std::vector<std::string> values = dbs[i].get_value(ft_atoi(tokens[2].c_str()) - 1);
                    for (unsigned int x = 0; x < values.size(); x++)
                    {
                        std::cout << values[x];
                        if (x < tokens.size() - 1)
                            std::cout << " ";
                    }
                    std::cout << std::endl;
                }
                else
                    std::cout << "No hay suficientes parametros" << std::endl;
                return;
            }
        }
    }
}

void db_listen()
{
    int client_size;
    SOCKET listen_sock, client_sock;
    struct sockaddr_in server;
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_sock == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
    server.sin_port = htons(5050);
    if (bind(listen_sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("bind failed: %d\n", WSAGetLastError());
        closesocket(listen_sock);
        WSACleanup();
        return;
    }
    //printf("La base de datos localhost:5050\n");
    while (true)
    {
        listen(listen_sock, SOMAXCONN);
        client_size = sizeof(struct sockaddr_in);
        client_sock = accept(listen_sock, (struct sockaddr*)&server, &client_size);
        std::thread server_db(db_sv, client_sock);
        server_db.detach();
    }

    WSACleanup();
}

int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    std::thread file_th(db_listen);
    file_th.detach();
    char msg[1024];
    while (true)
    {
        std::cout << "> ";
        std::cin.getline(msg, 1024);
        if (msg[0] != NULL)
        {
            std::string test = msg;
            std::vector<std::string> tokens = tokenize(test);
            evaluate(tokens);
        }
    }
    return 0;
}
