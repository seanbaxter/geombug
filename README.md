https://github.com/seanbaxter/geombug 
Build like this: 
```
clang++ geom.cxx -lGL -lglfw -lgl3w -o good
clang++ geom.cxx -lGL -lglfw -lgl3w -DBAD -o bad
```

The good executable draws a triangle. The bad executable does not. 

![good](https://raw.githubusercontent.com/seanbaxter/geombug/master/good.png) 
![bad](https://raw.githubusercontent.com/seanbaxter/geombug/master/bad.png)

Same program, but good sources good.spv and bad sources bad.spv. The only difference to my eyes is the OpCompositeConstruct in bad.spv that extends the vec3 vertex array to a vec4 for glgeom_Output.Position. 

https://github.com/seanbaxter/geombug/blob/master/geom.cxx#L11 

Above is the shader that generates the two spv. You see the only difference is the vec3 vs vec4 vertex array. Both of these executables work on Intel. bad.spv passes spirv-val. However I suspect that the root cause is that the NV driver doesn't like my placement of OpLoopMerge before the OpBranchConditional. 

glslang emits an additional basic block after the loop header but before the selection construct. I can't make heads or tails what the SPIRV spec is saying here. The code I generate looks good to me. Intel runs it. But if you tell me I'm misreading the SPIRV spec I'd believe you. 

So my suspicion is the OpLoopMerge position or structure of its selection construct... What about that OpCompositeConstruct? Well that's the weird part. In preparing this test case I pared away everything to get the smallest instruction, and when changing the vertex array from vec3 to vec4 I was able to drop the OpCompositeConstruct, and then it started working. I don't know if that's evidence against my OpLoopMerge theory, or evidence of an unrelated driver bug.