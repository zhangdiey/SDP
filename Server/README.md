# TODO

## Gymulator

## Error Correction

- [x] define “correct(detected_colour,last_instruction,remaining_instructions)” which returns a correction route
- [ ] implement it
- [ ] label networkx graph with colours

## Graph Colouring

- [ ] Colour physical graph
- [ ] Implement basic algorithm
- [ ] Implement algorithm to avoid recolouring

#psuedocode for correct
```
correct(detected_colour,last_instruction,remaining_instructions):
  guess_node= nearest node with colour attribute equal to detected colour
  if guess_node in remaining_instructions:
    return remaining instructions after guess node
  else: 
    return (route from guess_node to remaining_instructions[0]) + remaining_instructions
```
