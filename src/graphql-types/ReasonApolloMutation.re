external cast :
  string =>
  {
    .
    "data": Js.Json.t,
    "loading": bool,
  } =
  "%identity";

[@bs.module] external gql : ReasonApolloTypes.gql = "graphql-tag";

type state =
  | NotCalled
  | Loading
  | Loaded(Js.Json.t)
  | Failed(Js.Promise.error);

type action =
  | Result(string)
  | Error(Js.Promise.error);

let sendMutation = (~client, ~mutation, ~send) => {
  let _ =
    Js.Promise.(
      resolve(
        client##mutate({
          "mutation": gql(. mutation##query),
          "variables": mutation##variables,
        }),
      )
      |> then_(value => {
           send(Result(value));
           resolve();
         })
      |> catch(error => {
           send(Error(error));
           resolve();
         })
    );
  ();
};

let component = ReasonReact.reducerComponent("ReasonApollo");

let make = (~client, children) => {
  ...component,
  initialState: () => NotCalled,
  reducer: (action, _state) =>
    switch (action) {
    | Result(result) =>
      let typedResult = cast(result)##data;
      ReasonReact.Update(Loaded(typedResult));
    | Error(error) => ReasonReact.Update(Failed(error))
    },
  render: ({send, state}) => {
    let mutate = mutationFactory =>
      sendMutation(~client, ~mutation=mutationFactory, ~send);
    children(mutate, state);
  },
};