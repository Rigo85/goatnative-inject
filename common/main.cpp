//
//  main.cpp
//  Injector
//
//  Created by Max Raskin on 1/17/15.
//  Copyright (c) 2015 Max Raskin. All rights reserved.
//

#include <iostream>
#include <memory>
#include <string>

#include "Injector/goatnative/Injector.h"

using std::cout;
using std::endl;
using std::shared_ptr;
using std::string;

using goatnative::Injector;

class IFile
{
	virtual void open() = 0;
};

class Win32File : public IFile
{
	void open() override
	{
		cout << "opening!!!" << endl;
	}
};

class IConcurrency
{
public:
    virtual void createMutex() = 0;
    virtual ~IConcurrency() = default;
};

class IFileSystem
{
public:
    virtual void writeFile(const string& fileName) = 0;
    virtual ~IFileSystem() = default;
};

class INotifier
{
public:
    virtual void notify(const string& message, const string& target) = 0;
    virtual ~INotifier() = default;
};

class Concurrency : public IConcurrency
{
public:
    void createMutex() override
    {
        cout << "Creating mutex" << endl;
    }
};

class FileSystem : public IFileSystem
{
public:
    void writeFile(const string& fileName) override
    {
        cout << "Writing" << fileName << endl;
    }
};

class Notifier : public INotifier
{
public:
    void notify(const string& message, const string& target) override
    {
        cout << "Notifying " << target << " with message: " << message << endl;
    }
    
};

class ServicesProvider
{
public:
    ServicesProvider(shared_ptr<IConcurrency> concurency,
                     shared_ptr<IFileSystem> fileSystem,
                     shared_ptr<INotifier> notifier)
                : _concurency(concurency), _fileSystem(fileSystem), _notifier(notifier)
    {
        
    }
    
    shared_ptr<IConcurrency> concurrency()
    {
        return _concurency;
    }
    
    shared_ptr<IFileSystem> filesystem()
    {
        return _fileSystem;
    }
    
    shared_ptr<INotifier> notifier()
    {
        return _notifier;
    }
    
private:
    shared_ptr<IConcurrency> _concurency;
    shared_ptr<IFileSystem> _fileSystem;
    shared_ptr<INotifier> _notifier;
};

////////////////////////////////////////////

void testSingleton()
{
    Injector injector;
    
    injector.registerSingleton<Notifier>();
    injector.registerInterface<INotifier, Notifier>();
    auto notifier = injector.getInstance<INotifier>();
    auto notifier2 = injector.getInstance<INotifier>();
    
    assert(notifier == notifier2);
    
}

void testBuildWholeGraph()
{
    
    Injector injector;
    
    injector.registerSingleton<Notifier>();
	injector.registerInterface<INotifier, Notifier>();
    
	injector.registerSingleton<Concurrency>();
	injector.registerInterface<IConcurrency, Concurrency>();
    
	injector.registerSingleton<FileSystem>();
	injector.registerInterface<IFileSystem, FileSystem>();
    
	injector.registerSingleton<ServicesProvider, IConcurrency, IFileSystem, INotifier>();
    
    auto services = injector.getInstance<ServicesProvider>();

    assert(services->notifier());
    assert(services->filesystem());
    assert(services->concurrency());
    
    assert(services->notifier().get() == injector.getInstance<INotifier>().get());
    assert(services->concurrency().get() == injector.getInstance<IConcurrency>().get());
    assert(services->filesystem().get() == injector.getInstance<IFileSystem>().get());
}


void testInterfaceBasedFactory()
{

	Injector injector;

	injector.registerClass<Notifier>();
	injector.registerInterface<INotifier, Notifier>();

	auto notifier1 = injector.getInstance<INotifier>();
	auto notifier2 = injector.getInstance<INotifier>();

	assert(notifier1.get() != notifier2.get());
}


void testFactory()
{
    Injector injector;
    
    injector.registerClass<Notifier>();
    assert(injector.getInstance<Notifier>().get() != injector.getInstance<Notifier>().get());
}

int main(int argc, const char * argv[]) {
    // TODO - move tests to Google Test
    testSingleton();
    testBuildWholeGraph();
    testFactory();
	testInterfaceBasedFactory();

    return 0;
}


