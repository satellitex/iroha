

# How to build test of "builder"

source code: test_builder.py

## Usage   
```
$ python3 test_builder.py
```

## Detail  

First argument is previous code  
Second is entity name like "Asset"  
Third is list of member.  
name is member's name  
type is member's type (in now, supports Type.STR,Type.INT,Type.BOOL)

```python
makeTase4Builder(code,"Asset", [
    {
        "name":"DomainName",
        "type":Type.STR
    },
    {
        "name":"Name",
        "type":Type.STR
    },
    {
        "name":"Options",
        "type":Type.STR
    }
])
```


## Generated 

This is generated code example.

```c++
TEST( Asset, OptionsRandom_NameRandom_DomainNameRandom) {

    auto asset = AssetBuiler()
      .setOptions("GY2GIOBWGM4GKMRUMQZTKOJRGJRDQNTGMM4DMZRYGJRWGMBQGFSA====")
      .setName("MEZWKNJUGEZTSY3EMFSTQNBQGQYWINDCGQ3TEZDCGFRGIOLEMRRQ====")
      .setDomainName("ME4WIY3EHFRDCNJRHAZDQM3EMMYWGYJVGA4WCMZVHA2GMNTCGE2Q====")
      .build();

    ASSERT_STREQ(asset->options->c_str(), "GY2GIOBWGM4GKMRUMQZTKOJRGJRDQNTGMM4DMZRYGJRWGMBQGFSA====")
    ASSERT_STREQ(asset->name->c_str(), "MEZWKNJUGEZTSY3EMFSTQNBQGQYWINDCGQ3TEZDCGFRGIOLEMRRQ====")
    ASSERT_STREQ(asset->domainname->c_str(), "ME4WIY3EHFRDCNJRHAZDQM3EMMYWGYJVGA4WCMZVHA2GMNTCGE2Q====")
}
```