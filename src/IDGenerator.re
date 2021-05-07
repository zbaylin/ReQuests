module Make = (()) => {
  let currentID = ref(0);
  let getID = () => {
    let id = currentID^;
    currentID := id + 1;
    id;
  };
};
