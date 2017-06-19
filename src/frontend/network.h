#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "lib/sdl.h"
#include "lib/sdlnet.h"
#include "system.h"
#include "utils.h"
#include <cstdint>

namespace Network
{
    #ifdef E0INTERNAL_NETWORK_H_SPECIAL_ACCESS
    void Initialize();
    void Cleanup();
    #endif

    struct Address : IPaddress
    {
        operator std::string() const
        {
            if (Utils::big_endian)
                return Str((host & 0xff000000) >> 24, '.',
                           (host & 0xff0000) >> 16, '.',
                           (host & 0xff00) >> 8, '.',
                           (host & 0xff), ':',
                           port);
            else
                return Str((host & 0xff), '.',
                           (host & 0xff00) >> 8, '.',
                           (host & 0xff0000) >> 16, '.',
                           (host & 0xff000000) >> 24, ':',
                           SDL_Swap16(port));
        }

        Address()
        {
            host = 0;
            port = 0;
        }
        Address(IPaddress o)
        {
            host = o.host;
            port = o.port;
        }
        Address(std::string host, uint16_t port)
        {
            if (SDLNet_ResolveHost(this, host.c_str(), port))
                Exceptions::Network::CantResolve(Str("Unable to resolve a host name: ", (host[0] ? host : "<null>"), ':', port));
        }
    };

    class Server
    {
        TCPsocket socket;
        SDLNet_SocketSet set;
        bool connection_pending;
        uint16_t port;

      public:

        Server()
        {
            socket = 0;
            // set = 0 // This is not needed.
            connection_pending = 0;
            port = 0;
        }
        Server(uint16_t server_port)
        {
            socket = 0;
            // set = 0 // This is not needed.
            connection_pending = 0;
            port = server_port;
            Open(port);
        }
        Server(const Server &) = delete;
        Server(Server &&o)
        {
            socket = o.socket;
            set = o.set;
            connection_pending = o.connection_pending;
            port = o.port;
            o.socket = 0;
            // o.set = 0 // This is not needed.
            o.connection_pending = 0;
            o.port = 0;

        }
        Server &operator=(const Server &) = delete;
        Server &operator=(Server &&o)
        {
            if (&o == this)
                return *this;
            Close();
            socket = o.socket;
            set = o.set;
            connection_pending = o.connection_pending;
            port = o.port;
            o.socket = 0;
            // o.set = 0 // This is not needed.
            o.connection_pending = 0;
            o.port = 0;
            return *this;
        }
        ~Server()
        {
            Close();
        }

        void Open(uint16_t server_port)
        {

            Address address;
            address.host = INADDR_NONE;
            if (Utils::big_endian)
                address.port = server_port;
            else
                address.port = SDL_Swap16(server_port);
            port = 0;

            if (socket)
            {
                connection_pending = 0;
                SDLNet_TCP_DelSocket(set, socket);
                SDLNet_TCP_Close(socket);
                socket = SDLNet_TCP_Open(&address);
                if (!socket)
                {
                    SDLNet_FreeSocketSet(set);
                    Exceptions::Network::CantCreateServer(Str(server_port));
                }
                if (!SDLNet_TCP_AddSocket(set, socket))
                    Sys::Error("Can't add a server socket to a set.");
            }
            else
            {
                socket = SDLNet_TCP_Open(&address);
                if (!socket)
                    Exceptions::Network::CantCreateServer(Str(server_port));
                set = SDLNet_AllocSocketSet(1);
                if (!set)
                    Sys::Error("Can't allocate a socket set.");
                if (!SDLNet_TCP_AddSocket(set, socket))
                    Sys::Error("Can't add a server socket to a set.");
            }
            port = server_port;
        }

        void Close()
        {
            if (socket)
            {
                connection_pending = 0;
                port = 0;
                SDLNet_TCP_DelSocket(set, socket);
                SDLNet_FreeSocketSet(set);
                SDLNet_TCP_Close(socket);
                socket = 0;
            }
        }

        operator bool() const
        {
            return socket;
        }

        uint16_t Port() const
        {
            return port;
        }

        bool ConnectionPending(int ms_timeout = 0)
        {
            if (!socket)
                return 0;
            if (connection_pending)
                return 1;
            int result = SDLNet_CheckSockets(set, ms_timeout);
            if (result == -1)
                Sys::Error("Can't check a socket set.");
            connection_pending = result;
            return connection_pending;
        }

        friend class Connection;
    };

    class Connection
    {
        TCPsocket socket;
        SDLNet_SocketSet set;
        bool has_data;
        Address address;

      public:

        Connection()
        {
            socket = 0;
            // set = 0 // This is not needed.
            has_data = 0;
            address = {};
        }
        Connection(Address ip)
        {
            socket = 0;
            // set = 0 // This is not needed.
            has_data = 0;
            Open(ip);
        }
        Connection(const Connection &) = delete;
        Connection(Connection &&o)
        {
            socket = o.socket;
            set = o.set;
            has_data = o.has_data;
            address = o.address;
            o.socket = 0;
            // o.set = 0 // This is not needed.
            o.has_data = 0;
            o.address = {};
        }
        Connection &operator=(const Connection &) = delete;
        Connection &operator=(Connection &&o)
        {
            if (&o == this)
                return *this;
            Close();
            socket = o.socket;
            set = o.set;
            has_data = o.has_data;
            address = o.address;
            o.socket = 0;
            // o.set = 0 // This is not needed.
            o.has_data = 0;
            o.address = {};
            return *this;
        }
        ~Connection()
        {
            Close();
        }

        void Open(Address ip)
        {
            if (ip.host == INADDR_NONE || ip.host == INADDR_ANY)
                Exceptions::Network::CantConnect("Invalid IP.");
            address = {};
            if (socket)
            {
                has_data = 0;
                SDLNet_TCP_DelSocket(set, socket);
                SDLNet_TCP_Close(socket);
                socket = SDLNet_TCP_Open(&ip);
                if (!socket)
                {
                    SDLNet_FreeSocketSet(set);
                    Exceptions::Network::CantConnect(ip);
                }
                if (!SDLNet_TCP_AddSocket(set, socket))
                    Sys::Error("Can't add a socket to a set.");
            }
            else
            {
                socket = SDLNet_TCP_Open(&ip);
                if (!socket)
                    Exceptions::Network::CantConnect(ip);
                set = SDLNet_AllocSocketSet(1);
                if (!set)
                    Sys::Error("Can't allocate a socket set.");
                if (!SDLNet_TCP_AddSocket(set, socket))
                    Sys::Error("Can't add a socket to a set.");
            }
            address = ip;
        }
        void Open(Server &server, int ms_timeout = 0)
        {
            if (!server.ConnectionPending(ms_timeout))
                return;
            address = {};
            if (socket)
            {
                has_data = 0;
                SDLNet_TCP_DelSocket(set, socket);
                SDLNet_TCP_Close(socket);
                socket = SDLNet_TCP_Accept(server.socket);
                if (!socket)
                {
                    SDLNet_FreeSocketSet(set);
                    return;
                }
                if (!SDLNet_TCP_AddSocket(set, socket))
                    Sys::Error("Can't add a socket to a set.");
            }
            else
            {
                socket = SDLNet_TCP_Accept(server.socket);
                if (!socket)
                    return;
                set = SDLNet_AllocSocketSet(1);
                if (!set)
                    Sys::Error("Can't allocate a socket set.");
                if (!SDLNet_TCP_AddSocket(set, socket))
                    Sys::Error("Can't add a socket to a set.");
            }
            address = *SDLNet_TCP_GetPeerAddress(socket);
        }
        void Close()
        {
            if (socket)
            {
                has_data = 0;
                address = {};
                SDLNet_TCP_DelSocket(set, socket);
                SDLNet_FreeSocketSet(set);
                SDLNet_TCP_Close(socket);
                socket = 0;
            }
        }

        operator bool() const
        {
            return socket;
        }

        Address Address()
        {
            return address;
        }

        bool DataAvailable(int ms_timeout = 0)
        {
            if (!socket)
                return 0;
            if (has_data)
                return 1;
            int result = SDLNet_CheckSockets(set, ms_timeout);
            if (result == -1)
                Sys::Error("Can't check a socket set.");
            has_data = result;
            return has_data;
        }

        int Receive(ArrayProxy<uint8_t> arr, int ms_timeout = 0) // Returns an amount of received bytes (or 0 if no data is available or if the socket was closed.).
        {
            if (!socket)
                return 0;
            if (!DataAvailable(ms_timeout))
                return 0;
            has_data = 0;
            int result = SDLNet_TCP_Recv(socket, arr, arr.size());
            if (result <= 0)
            {
                // A error has occured. We silently close the socket.
                Close();
            }
            return (result > 0 ? result : 0);
        }

        bool Send(ArrayView<uint8_t> arr) // Returns 1 on success.
        {
            if (!socket)
                return 0;
            int result = SDLNet_TCP_Send(socket, arr, arr.size());
            if (result < int(arr.size()))
            {
                // We can't send a data, so we assume a error has occured and we silently close the socket.
                Close();
                return 0;
            }
            return 1;
        }
    };
}

#endif
