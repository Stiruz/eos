#pragma once

#include <optional>                                   /* optional */            

#include <boost/asio/ssl/context.hpp>                 /* ssl::context */

#include <eosio/chain/thread_utils.hpp>               /* named_thread_pool */
#include <eosio/web_server_plugin/http_server_interface.hpp>   /* http_server_interface */
#include <eosio/web_server_plugin/beast_listener.hpp> /* http_listener/https_listener */


namespace eosio::web::beastimpl{

struct http_server : virtual http_server_interface{
   using handler_map = http_listener::handler_map;

   http_server();
   virtual ~http_server();

   //<http_server_interface>
   virtual void init(server_address&& address, boost::asio::io_context* context);
   virtual void init(server_address&& address, uint8_t thread_pool_size);

   /**
    * @brief this method is for building an API for the server.
    * Adding handlers permitted only before executing http_server::run method.
    * Reason for this is to gain performance from direct access without 
    * using synchronization primitives while reading from the internal map.
    * @param path http path. server first checks for exact match and if not found
    * it uses string::starts_with so if you specify partial path
    * it will be called for all requests where path starts from this string
    * @param callback callback to fill out client's response. server get's 
    * related data through the callback parameters (method, other headers, etc)
    * @throw other_http_exception if server is already running
    */
   virtual void add_handler(std::string_view path, method_type method, server_handler callback);
   /**
    * this method starts listening socket and accepting connections 
    */
   virtual void run();
   //</http_server_interface>

   server_address address;

protected:
   bool                                            running;
   handler_map                                     handlers;
   boost::asio::io_context*                        context;
   std::optional<eosio::chain::named_thread_pool>  thread_pool;

   std::optional<http_listener>  listener;

   inline boost::asio::io_context& get_executor() {
      EOS_ASSERT(context || thread_pool, eosio::chain::other_http_exception, "both context and thread_pool not initialized");
      return context ? *context : thread_pool->get_executor();
   }
};

struct https_server : http_server, https_server_interface{
   using ssl_context            = boost::asio::ssl::context;
   

   https_server();
   virtual ~https_server();
   //<http_server_interface>
   virtual void run();
   //rest methods are implemented in http_server
   //</http_server_interface>

   //<https_server_interface>
   /**
    * @param cert certificate in PEM format
    * @param pk server's private ket
    * @param pwd_callback password callback. optional
    * @param dh parameters for Ephemeral Diffie-Hellman key agreement protocol. optional
    * if not specified static Diffie-Hellman protocol will be used.
    */
   virtual void init_ssl(std::string_view cert, 
                         std::string_view pk, 
                         password_callback pwd_callback = empty_callback, 
                         std::string_view dh = {});
   //</https_server_interface>
protected:
   ssl_context    ssl_ctx;
   std::optional<https_listener> ssl_listener;
};

struct web_server_factory : http_server_factory_interface{
   web_server_factory();
   virtual ~web_server_factory();

   //caller is responsible for deletion of server object
   virtual http_server_interface* create_server(server_address&& address, boost::asio::io_context* context);
};

}