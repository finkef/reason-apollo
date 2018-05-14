let createApolloClient =
    (
      ~cache,
      ~link,
      ~ssrMode=?,
      ~ssrForceFetchDelay=?,
      ~connectToDevTools=?,
      ~queryDeduplication=?,
      (),
    ) => {
  let apolloClientOptions = {
    "link": link,
    "cache": cache,
    "ssrMode": Js.Nullable.fromOption(ssrMode),
    "ssrForceFetchDelay": Js.Nullable.fromOption(ssrForceFetchDelay),
    "connectToDevTools": Js.Nullable.fromOption(connectToDevTools),
    "queryDeduplication": Js.Nullable.fromOption(queryDeduplication),
  };
  ApolloClient.createApolloClient(apolloClientOptions);
};

module type ApolloClientConfig = {
  let apolloClient: ApolloClient.generatedApolloClient;
};

module CreateClient = (Config: ApolloClientConfig) => {
  let apolloClient = Config.apolloClient;
  /*
   * Expose a module to perform "query" operations for the given client
   */
  module Query =
    ReasonApolloFactories.QueryFactory(
      {
        let apolloClient = apolloClient;
      },
    );
  /*
   * Expose a module to perform "mutation" operations for the given client
   */
  module Mutation =
    ReasonApolloFactories.MutationFactory(
      {
        let apolloClient = apolloClient;
      },
    );
};