# Offer Flowers Faster

This project created a Minecraft: Bedrock Edition mod that modifies the probability of iron golems offering poppies to baby villagers and copper golems, making it easier to observe the related phenomenon.

This mod uses a simpler way to increase the original 1/ 8000 probability, which is to call the `canUse()` function 4000 times when the game calls it. At this time, the probability of success is $1 - (\frac{7999}{8000})^{4000} ≈ 39.35\%$.

The project is based on [LeviLamina](https://levimc.org/software/levilamina); see [LeviLamina Docs](https://lamina.levimc.org/) for more information.

## License

The mod is released under the [Apache 2.0 license](LICENSE).

``` text
  Copyright 2025 SkyEye_FAST

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
```

## Feedback

Please feel free to raise issues for any problems encountered or feature suggestions.

Pull requests are welcome.