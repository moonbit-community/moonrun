// Copyright 2024 International Digital Economy Academy
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use std::error::Error;
use wasmtime;
use clap::{arg, command};
use std::path::PathBuf;

fn main() -> Result<(), Box<dyn Error>> {
    let matches = command!().arg(
        arg!(<PATH>)
            .value_parser(clap::value_parser!(PathBuf))
            .required(true),
    ).get_matches();

    let file = matches.get_one::<PathBuf>("PATH").unwrap();

    let engine = wasmtime::Engine::default();
    let module = wasmtime::Module::from_file(&engine, file)?;

    let mut linker = wasmtime::Linker::new(&engine);

    let dangling_surrogate_high = std::sync::Mutex::new(-1);

    linker.func_wrap("spectest", "print_char", move |c: i32| {
        if c >= 0xd800 && c <= 0xdbff {
            let mut high= dangling_surrogate_high.lock().unwrap();
            assert!(*high == -1);
            *high = c - 0xd800;
        } else {
            let c = {
                let mut high= dangling_surrogate_high.lock().unwrap();
                if *high != -1 {
                    assert!(c >= 0xdc00 && c <= 0xdfff);
                    let x = 0x10000 + (*high << 10) + (c - 0xdc00);
                    *high = -1;
                    x
                } else {
                    c
                }
            };
            let c = std::char::from_u32(c as u32).unwrap();
            print!("{}", c);
        }
    })?;

    let mut store = wasmtime::Store::new(&engine, ());
    let instance = linker.instantiate(&mut store, &module)?;

    let run = instance.get_typed_func::<(), ()>(&mut store, "_start")?;
    run.call(&mut store, ())?;

    Ok(())
}
