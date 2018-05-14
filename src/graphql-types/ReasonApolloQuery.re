external castResponse : string => {. "data": Js.Json.t} = "%identity";

external asJsObject : 'a => Js.t({..}) = "%identity";

[@bs.module] external gql : ReasonApolloTypes.gql = "graphql-tag";

[@bs.module]
external shallowEqual : (Js.t({..}), Js.t({..})) => bool =
  "fbjs/lib/shallowEqual";

type response =
  | Loading
  | Loaded(Js.Json.t)
  | Failed(string);

type state = {
  response,
  variables: Js.Json.t,
};

type action =
  | Result(string)
  | Error(string);

let sendQuery = (~client, ~query, ~send) => {
  let _ =
    Js.Promise.(
      resolve(
        client##query({
          "query": gql(. query##query),
          "variables": query##variables,
        }),
      )
      |> then_(value => {
           send(Result(value));
           resolve();
         })
      |> catch(_value => {
           send(Error("an error happened"));
           resolve();
         })
    );
  ();
};

let component = ReasonReact.reducerComponent("ReasonApollo");

let make = (~client, ~query as q, children) => {
  ...component,
  initialState: () => {response: Loading, variables: q##variables},
  reducer: (action, state) =>
    switch (action) {
    | Result(result) =>
      let typedResult = castResponse(result)##data;
      ReasonReact.Update({...state, response: Loaded(typedResult)});
    | Error(error) =>
      ReasonReact.Update({...state, response: Failed(error)})
    },
  willReceiveProps: ({state, send}) =>
    if (!
          shallowEqual(
            asJsObject(q##variables),
            asJsObject(state.variables),
          )) {
      sendQuery(~client, ~query=q, ~send);
      state;
    } else {
      state;
    },
  didMount: ({send}) => {
    sendQuery(~client, ~query=q, ~send);
    ();
  },
  render: ({state}) => children(state.response, q##parse),
};